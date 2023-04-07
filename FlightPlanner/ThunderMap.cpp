#include <cmath>
#include <vector>
#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

#include "ThunderCore.h"
#include "ThunderMap.h"
#include "ThunderNet.h"

int Thunder::Map::Camera::getZoom() { return zoom; }
int Thunder::Map::Camera::getX() { return x; }
int Thunder::Map::Camera::getY() { return y; }
void Thunder::Map::Camera::zoomIn() { if (zoom < 19) zoom++; }
void Thunder::Map::Camera::zoomOut() { if (zoom > 0) zoom--; }

void Thunder::Map::Camera::move(int incX, int incY)
{
	x += incX;
	y += incY;
}

Thunder::Map::Tile::Pos Thunder::Map::Tile::getPos() { return pos; }
Thunder::Map::Tile::Coords Thunder::Map::Tile::getCoords() { return coords; }

Thunder::Map::Tile::Coords Thunder::Map::Tile::Pos::toCoords(int zoom)
{
	double n{ M_PI - 2 * M_PI * y / static_cast<double>(1 << zoom) };
	return { x / static_cast<double>(1 << zoom) * 360 - 180, 180 / M_PI * atan(0.5 * (exp(n) - exp(-n))) };
}

Thunder::Map::Tile::Pos Thunder::Map::Tile::Coords::toPos(int zoom)
{
	double latRad{ lat * M_PI / 180 };
	return { static_cast<int>(floor((lon + 180) / 360 * static_cast<double>(1 << zoom))), static_cast<int>(floor((1 - asinh(tan(latRad)) / M_PI) / 2 * static_cast<double>(1 << zoom))) };
}

void Thunder::Map::draw()
{
	for (const Tile& tile : tiles)
		tile.draw();
}

void Thunder::Map::Tile::load(std::vector<char> content)
{
	SDL_RWops* img{ SDL_RWFromMem(content.data() + 690, 10000) };
	tiles.push_back({ 0, 0, IMG_LoadTexture_RW(renderer, img, 0) });
	SDL_RWclose(img);
}

void Thunder::Map::Tile::draw() const
{
	SDL_Rect rect{ pos.x, pos.y, 256, 256 };
	SDL_RenderCopy(renderer, img, NULL, &rect);
}

Thunder::Map::Tile::Tile(int x, int y, SDL_Texture* texture) : img{ texture }
{
	pos.x = x;
	pos.y = y;
}