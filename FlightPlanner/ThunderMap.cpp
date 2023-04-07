#include "ThunderMap.h"

int Thunder::Map::Camera::getZoom() { return zoom; }

int Thunder::Map::Camera::getX() { return x; }

int Thunder::Map::Camera::getY() { return y; }

void Thunder::Map::Camera::move(int incX, int incY)
{
	x += static_cast<double>(incX) / (zoom + 1);
	y += static_cast<double>(incY) / (zoom + 1);
}

void Thunder::Map::Camera::zoomIn()
{
	if (zoom < 19)
		zoom++;
}

void Thunder::Map::Camera::zoomOut()
{
	if (zoom > 0)
		zoom--;
}