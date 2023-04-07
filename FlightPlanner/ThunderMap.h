#ifndef THUNDER_MAP
#define THUNDER_MAP

#include <SDL.h>

namespace Thunder::Map
{
	class Camera
	{
	private:
		int zoom{};
		int x{};
		int y{};
	public:
		int getZoom();
		int getX();
		int getY();
		void zoomIn();
		void zoomOut();
		void move(int, int);
	} camera{};

	class Tile
	{
	private:
		struct Pos;
		struct Coords;

		struct Pos
		{
			int x{};
			int y{};
			Coords toCoords(int);
		} pos{};

		struct Coords
		{
			double lat{};
			double lon{};
			Pos toPos(int);
		} coords{};

		SDL_Texture* img{};
	public:
		Pos getPos();
		Coords getCoords();
	};
}

#endif