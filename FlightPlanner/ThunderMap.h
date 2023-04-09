#ifndef THUNDER_MAP
#define THUNDER_MAP

#include <string>
#include <vector>
#include <unordered_set>

#include <SDL.h>

namespace Thunder::Map
{
	struct Pos;
	struct Coords;

	struct Pos
	{
		int x{};
		int y{};
		Coords toCoords(int) const;
	};

	struct Coords
	{
		double lon{};
		double lat{};
		Pos toPos(int) const;
	};

	inline class Camera
	{
	private:
		int zoom{};
		int x{};
		int y{};
	public:
		int getZoom() const;
		int getX() const;
		int getY() const;
		void zoomIn();
		void zoomOut();
		void move(int, int);
	} camera{};

	class Tile
	{
	private:
		int zoom{};
		Pos pos{};
		Coords coords{};
		std::vector<char> rawTexture{};
	public:
		struct HashFunc
		{
			size_t operator()(const Tile&) const;
		};
		bool operator==(const Tile&) const;
		Tile(int, int, int, std::vector<char>);
		Pos getPos() const;
		Coords getCoords() const;
		int getZoom() const;
		void draw() const;
	};

	struct ThreadData
	{
		int x{};
		int y{};
		int i{};
	};
	struct Thread
	{
		SDL_Thread* thread{ nullptr };
		bool threadDone{ true };
	};
	inline Thread updateThreads[100]{};
	inline std::unordered_set<Tile, Tile::HashFunc> tiles{};

	void draw();
	void updateTiles();
	int addTile(void*);
}

#endif