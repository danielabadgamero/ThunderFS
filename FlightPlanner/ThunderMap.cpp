#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <filesystem>

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
		int mouseX{};
		int mouseY{};
		SDL_GetMouseState(&mouseX, &mouseY);
		x += mouseX;
		y += mouseY;
		zoom++;
	}
}

void Thunder::Map::Camera::zoomOut()
{
	if (zoom > 0)
	{
		int mouseX{};
		int mouseY{};
		SDL_GetMouseState(&mouseX, &mouseY);
		x -= mouseX;
		y -= mouseY;
		x /= 2;
		y /= 2;
		zoom--;
	}
}

void Thunder::Map::Camera::move(int incX, int incY)
{
	x -= incX;
	y -= incY;
}

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

bool Thunder::Map::Pos::operator==(const Pos& B) const
{
	return x == B.x && y == B.y;
}

void Thunder::Map::updateTiles()
{
	for (int startY{}; startY != monitor.h / 256 + 1; startY++)
		for (int startX{}; startX != monitor.w / 256 + 1; startX++)
		{
			int max{ static_cast<int>(std::pow(2, camera.getZoom())) };
			if (!tiles[camera.getZoom()].contains({ mod(startX + camera.getX() / 256, max), mod(startY + camera.getY() / 256, max)}))
				for (int i{}; i != 200; i++)
					if (updateThreads[i].threadDone)
					{
						SDL_WaitThread(updateThreads[i].thread, nullptr);
						updateThreads[i].threadDone = false;
						updateThreads[i].thread = SDL_CreateThread(addTile, std::string{ "update_#" + std::to_string(i) }.c_str(), (void*)(new ThreadData
							{ mod(startX + camera.getX() / 256, max), mod(startY + camera.getY() / 256, max), i }));
						break;
					}
		}
}

int Thunder::Map::addTile(void* data)
{
	ThreadData pos{ *(ThreadData*)data };

	std::string URL{ "/" + std::to_string(camera.getZoom()) + "/" + std::to_string(pos.x) + "/" + std::to_string(pos.y) + ".png" };
	std::string path{ std::to_string(camera.getZoom()) + "_" + std::to_string(pos.x) + "_" + std::to_string(pos.y) + ".png" };

	std::vector<char> content{};
	if (std::filesystem::exists("./tiles/" + path))
	{
		std::ifstream tile{ "./tiles/" + path, std::ios::binary };
		while (!tile.eof())
		{
			content.push_back(0);
			tile.read(&content.back(), 1);
		}
		tiles[camera.getZoom()][{ pos.x, pos.y }] = new Tile{ content };
	}
	else
	{
		Net::send(URL);
		content = Net::receive();
		if (content.size() > 1)
		{
			tiles[camera.getZoom()][{ pos.x, pos.y }] = new Tile{ content };
			std::ofstream tile{ "./tiles/" + path, std::ios::binary };
			for (std::vector<char>::iterator PNG_start{ std::find(content.begin(), content.end(), -119) }; PNG_start != content.end(); PNG_start++)
				tile.write(&(*PNG_start), 1);
		}
	}

	updateThreads[pos.i].threadDone = true;

	return 0;
}

void Thunder::Map::draw()
{
	for (const auto& [pos, tile] : tiles[camera.getZoom()])
		if (tile->inCamera(pos))
			tile->draw(pos);
}

bool Thunder::Map::Tile::inCamera(Pos pos) const
{
	SDL_Point tilePoint{ pos.x, pos.y };
	SDL_Rect cameraRect{ -256, -256, monitor.w + 256, monitor.h + 256 };
	return SDL_PointInRect(&tilePoint, &cameraRect);
}

void Thunder::Map::Tile::draw(Pos pos) const
{
	SDL_RWops* img{ SDL_RWFromMem((void*)rawTexture.data(), static_cast<int>(rawTexture.end() - rawTexture.begin()))};
	SDL_Texture* texture{ IMG_LoadTexture_RW(renderer, img, 0) };
	SDL_RWclose(img);
	SDL_Rect rect{ pos.x * 256 - camera.getX(), pos.y * 256 - camera.getY(), 256, 256 };
	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_DestroyTexture(texture);
}

Thunder::Map::Tile::Tile(std::vector<char> content)
{
	std::vector<char>::iterator startIt{ std::find(content.begin(), content.end(), -119) };
	if (startIt == content.end())
		return;
	for (; startIt != content.end(); startIt++)
		rawTexture.push_back(*startIt);
}

unsigned long long Thunder::Map::HashFunc::operator()(const Pos& pos) const
{
	return (std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.y) << 1));
}