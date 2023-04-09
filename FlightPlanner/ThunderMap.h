#ifndef THUNDER_MAP
#define THUNDER_MAP

#include <string>
#include <vector>
#include <unordered_map>

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
		bool operator==(const Pos&) const;
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
		int zoom{ 1 };
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
		std::vector<char> rawTexture{};
	public:
		Tile(std::vector<char>);
		void draw(Pos) const;
		std::vector<char> getTexture() const;
	};

	inline Thread update{};

	struct HashFunc
	{
		unsigned long long operator()(const Pos&) const;
	};
	inline std::unordered_map<Pos, Tile*, HashFunc> tiles[20]{};

	int loadCache(void*);
	void draw();
	void updateTiles();
	int addTile(void*);
	int saveCache(void*);
}

#endif