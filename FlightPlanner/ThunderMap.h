#ifndef THUNDER_MAP
#define THUNDER_MAP

#include <string>
#include <vector>

#include <SDL.h>

namespace Thunder::Map
{
	struct Pos;
	struct Coords;

	struct Pos
	{
		int x{};
		int y{};
		Coords toCoords(int);
	};

	struct Coords
	{
		double lon{};
		double lat{};
		Pos toPos(int);
	};

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
	public:
		struct Data
		{
			int zoom{};
			Pos pos{};
		};
	private:
		Data data{};
		Coords coords{};
		SDL_Texture* texture{};
	public:
		Tile(int, int, std::vector<char>);
		Pos getPos();
		Coords getCoords();
		int getZoom();
		void draw() const;

		bool operator==(Data);
	};

	inline std::vector<Tile> tiles{};

	void draw();
}

#endif