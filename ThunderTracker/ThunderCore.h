#ifndef THUNDER_CORE
#define THUNDER_CORE

#include <SDL.h>

namespace Thunder
{
	inline SDL_Window* window{};
	inline SDL_Renderer* renderer{};
	inline SDL_DisplayMode monitor{};

	inline bool running{};

	void init(const char*);
	void event();
	void draw();
	void quit();
}

#endif