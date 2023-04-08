#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

#include <SDL.h>
#include <SDL_image.h>

#include "ThunderCore.h"
#include "ThunderMap.h"
#include "ThunderNet.h"

int Thunder::Map::Camera::getZoom() { return zoom; }
int Thunder::Map::Camera::getX() { return x; }
int Thunder::Map::Camera::getY() { return y; }
void Thunder::Map::Camera::zoomIn() { if (zoom < 19) zoom++; tiles.clear(); }
void Thunder::Map::Camera::zoomOut() { if (zoom > 0) zoom--; tiles.clear(); }

void Thunder::Map::Camera::move(int incX, int incY)
{
	x += incX;
	y += incY;
}

Thunder::Map::Pos Thunder::Map::Tile::getPos() { return data.pos; }
Thunder::Map::Coords Thunder::Map::Tile::getCoords() { return coords; }

Thunder::Map::Coords Thunder::Map::Pos::toCoords(int zoom)
{
	double n{ M_PI - 2 * M_PI * y / static_cast<double>(1 << zoom) };
	return { x / static_cast<double>(1 << zoom) * 360 - 180, 180 / M_PI * atan(0.5 * (exp(n) - exp(-n))) };
}

Thunder::Map::Pos Thunder::Map::Coords::toPos(int zoom)
{
	double latRad{ lat * M_PI / 180 };
	return { static_cast<int>(floor((lon + 180) / 360 * static_cast<double>(1 << zoom))), static_cast<int>(floor((1 - asinh(tan(latRad)) / M_PI) / 2 * static_cast<double>(1 << zoom))) };
}

void Thunder::Map::draw()
{
	Pos startPos{ camera.getX() / 256, camera.getY() / 256 };
	Pos endPos{ (camera.getX() + monitor.w) / 256, (camera.getY() + monitor.h) / 256 };

	for (; startPos.y != endPos.y; startPos.y++)
		for (; startPos.x != endPos.x; startPos.x++)
		{
			Net::send("/" + std::to_string(camera.getZoom()) + "/" + std::to_string(startPos.x) + "/" + std::to_string(startPos.y) + ".png");
			std::vector<char> content{ Net::receive() };
			if (std::find(tiles.begin(), tiles.end(), Tile::Data{ camera.getZoom(), startPos.x, startPos.y }) == tiles.end())
				tiles.push_back({ startPos.x, startPos.y, content });
		}

	for (const Tile& tile : tiles)
		tile.draw();
}

void Thunder::Map::Tile::draw() const
{
	SDL_Rect rect{ data.pos.x + camera.getX(), data.pos.y + camera.getY(), 256, 256 };
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

Thunder::Map::Tile::Tile(int x, int y, std::vector<char> content)
{
	std::vector<char>::iterator startIt{ std::find(content.begin(), content.end(), -119) };
	if (startIt == content.end())
		return;
	char* start{ &(*startIt) };
	SDL_RWops* img{ SDL_RWFromMem(start, static_cast<int>(content.end() - startIt)) };
	texture = IMG_LoadTexture_RW(renderer, img, 0);
	SDL_RWclose(img);

	data.pos.x = x * 256;
	data.pos.y = y * 256;
}

bool Thunder::Map::Tile::operator==(Thunder::Map::Tile::Data B) { return (data.zoom == B.zoom) && (data.pos.x == B.pos.x) && (data.pos.y == B.pos.y); }
int Thunder::Map::Tile::getZoom() { return data.zoom; }