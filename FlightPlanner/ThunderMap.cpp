#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_set>

#include <SDL.h>
#include <SDL_image.h>

#include "ThunderCore.h"
#include "ThunderMap.h"
#include "ThunderNet.h"

static int mod(int x, int y)
{
	if (y == 0) exit(-1);
	if (y == -1) return 0;
	return x - y * (x / y - (x % y && (x ^ y) < 0));
}

int Thunder::Map::Camera::getZoom() const { return zoom; }
int Thunder::Map::Camera::getX() const { return x; }
int Thunder::Map::Camera::getY() const { return y; }

void Thunder::Map::Camera::zoomIn()
{
	if (zoom < 19)
	{
		x *= 2;
		y *= 2;
		zoom++;
		if (threadStatus)
		{
			threadStatus = 0;
			SDL_WaitThread(updateThread, nullptr);
			updateThread = SDL_CreateThread(updateTiles, "updateTiles", nullptr);
		}
	}
}

void Thunder::Map::Camera::zoomOut()
{
	if (zoom > 0)
	{
		x /= 2;
		y /= 2;
		zoom--;
		if (threadStatus)
		{
			threadStatus = 0;
			SDL_WaitThread(updateThread, nullptr);
			updateThread = SDL_CreateThread(updateTiles, "updateTiles", nullptr);
		}
	}
}

void Thunder::Map::Camera::move(int incX, int incY)
{
	x -= incX;
	y -= incY;

	if (threadStatus)
	{
		threadStatus = 0;
		SDL_WaitThread(updateThread, nullptr);
		updateThread = SDL_CreateThread(updateTiles, "updateTiles", nullptr);
	}
}

Thunder::Map::Pos Thunder::Map::Tile::getPos() const { return pos; }
Thunder::Map::Coords Thunder::Map::Tile::getCoords() const { return coords; }

Thunder::Map::Coords Thunder::Map::Pos::toCoords(int zoom) const
{
	double n{ M_PI - 2 * M_PI * y / static_cast<double>(1 << zoom) };
	return { x / static_cast<double>(1 << zoom) * 360 - 180, 180 / M_PI * atan(0.5 * (exp(n) - exp(-n))) };
}

Thunder::Map::Pos Thunder::Map::Coords::toPos(int zoom) const
{
	double latRad{ lat * M_PI / 180 };
	return { static_cast<int>(floor((lon + 180) / 360 * static_cast<double>(1 << zoom))), static_cast<int>(floor((1 - asinh(tan(latRad)) / M_PI) / 2 * static_cast<double>(1 << zoom))) };
}

int Thunder::Map::updateTiles(void*)
{
	Pos endPos{ (monitor.w + camera.getX()) / 256 + 1, (monitor.h + camera.getY()) / 256 + 1 };

	for (int startY{ camera.getY() / 256 }; startY != endPos.y; startY++)
		for (int startX{ camera.getX() / 256 }; startX != endPos.x; startX++)
			if (!tiles.contains(Tile{ camera.getZoom(), startX, startY, {} }))
			{
				int max{ static_cast<int>(std::pow(2, camera.getZoom())) };
				if (startX >= max)
					break;
				if (startY >= max)
					goto end;
				Net::send("/" + std::to_string(camera.getZoom()) + "/" + std::to_string(mod(startX, max)) + "/" + std::to_string(mod(startY, max)) + ".png");
				std::vector<char> content{ Net::receive() };
				if (content.size() > 1)
					tiles.emplace(Tile{ camera.getZoom(), mod(startX, max), mod(startY, max), content});
			}
end:
	threadStatus = 1;
	return 0;
}

void Thunder::Map::draw()
{
	for (std::unordered_set<Tile>::iterator tile{ tiles.begin() }; tile != tiles.end();)
		if (tile->getZoom() == camera.getZoom())
		{
			SDL_Rect cameraRect{ camera.getX() - 512, camera.getY() - 512, monitor.w + 512, monitor.h + 512 };
			SDL_Point tilePoint{ tile->getPos().x * 256, tile->getPos().y * 256 };
			if (SDL_PointInRect(&tilePoint, &cameraRect))
			{
				tile->draw();
				tile++;
			}
			else
				tiles.erase(tile++);
		}
		else
			tiles.erase(tile++);

}

void Thunder::Map::Tile::draw() const
{
	SDL_RWops* img{ SDL_RWFromMem((void*)rawTexture.data(), static_cast<int>(rawTexture.end() - rawTexture.begin()))};
	SDL_Texture* texture{ IMG_LoadTexture_RW(renderer, img, 0) };
	SDL_RWclose(img);
	if (texture)
	{
		SDL_Rect rect{ pos.x * 256 - camera.getX(), pos.y * 256 - camera.getY(), 256, 256 };
		SDL_RenderCopy(renderer, texture, NULL, &rect);
	}
	SDL_DestroyTexture(texture);
}

Thunder::Map::Tile::Tile(int zoom, int x, int y, std::vector<char> content) : zoom{ zoom }
{
	pos.x = x;
	pos.y = y;

	std::vector<char>::iterator startIt{ std::find(content.begin(), content.end(), -119) };
	if (startIt == content.end())
		return;
	for (; startIt != content.end(); startIt++)
		rawTexture.push_back(*startIt);
}

int Thunder::Map::Tile::getZoom() const { return zoom; }
bool Thunder::Map::Tile::operator==(const Tile& tile) const { return (zoom == tile.zoom) && (pos.x == tile.pos.x) && (pos.y == tile.pos.y); }

size_t Thunder::Map::Tile::HashFunc::operator()(const Tile& tile) const
{
	size_t zHash{ std::hash<int>()(tile.zoom) };
	size_t xHash{ std::hash<int>()(tile.pos.x) << 1 };
	size_t yHash{ std::hash<int>()(tile.pos.y) << 2 };
	return zHash ^ xHash ^ yHash;
}