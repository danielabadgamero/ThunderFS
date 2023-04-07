#ifndef THUNDER_MAP
#define THUNDER_MAP

#include <string>
#include <vector>

#include <SDL.h>

namespace Thunder::Map
{
	inline class Camera
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
			double lon{};
			double lat{};
			Pos toPos(int);
		} coords{};

		SDL_Texture* texture{};
	public:
		Tile(int, int, std::vector<char>);
		Pos getPos();
		Coords getCoords();
		void draw() const;
	};

	inline std::vector<Tile> tiles{};

	void event();
	void draw();
}

#endif