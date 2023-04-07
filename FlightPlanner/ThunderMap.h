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

		SDL_Texture* img{};
	public:
		Tile(int, int, SDL_Texture*);
		Pos getPos();
		Coords getCoords();
		void load(std::vector<char>);
		void draw() const;
	};

	inline std::vector<Tile> tiles{};

	void init();
	void draw();
}

#endif