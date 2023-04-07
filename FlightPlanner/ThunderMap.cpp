#include <cmath>

#include "ThunderMap.h"
#include "ThunderNet.h"

int Thunder::Map::Camera::getZoom() { return zoom; }
int Thunder::Map::Camera::getX() { return x; }
int Thunder::Map::Camera::getY() { return y; }
void Thunder::Map::Camera::zoomIn() { if (zoom < 19) zoom++; }
void Thunder::Map::Camera::zoomOut() { if (zoom > 0) zoom--; }

void Thunder::Map::Camera::move(int incX, int incY)
{
	x += static_cast<double>(incX) / (zoom + 1);
	y += static_cast<double>(incY) / (zoom + 1);
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

void Thunder::Map::init()
{
	Net::connect("tile.openstreetmap.org");
	Net::send("/12/2044/1556.png");
	std::string response{ Net::receive() };
	SDL_Log(response.c_str());
	Net::close();
}