#ifndef THUNDER_CORE
#define THUNDER_CORE

#include <SDL.h>

namespace Thunder
{
	struct Thread
	{
		SDL_Thread* thread{};
		bool done{};
	};

	inline Thread load{};

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