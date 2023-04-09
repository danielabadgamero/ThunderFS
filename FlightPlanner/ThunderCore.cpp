#include <SDL.h>
#include <SDL_image.h>
#include <SDL_net.h>

#include "ThunderCore.h"
#include "ThunderMap.h"
#include "ThunderNet.h"

void Thunder::init(const char* title)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	SDLNet_Init();

	SDL_GetCurrentDisplayMode(0, &monitor);
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, monitor.w, monitor.h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	running = true;

	Map::loadCache();
	Net::connect("tile.openstreetmap.org");
}

void Thunder::event()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
		switch (e.type)
		{
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.scancode)
			{
			case SDL_SCANCODE_ESCAPE:
				running = false;
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LEFT)
				Map::camera.move(e.motion.xrel, e.motion.yrel);
			break;
		case SDL_MOUSEWHEEL:
			if (e.wheel.y > 0)
				Map::camera.zoomIn();
			else
				Map::camera.zoomOut();
			break;
		}
}

void Thunder::draw()
{
	Map::updateTiles();

	SDL_SetRenderDrawColor(renderer, 0x10, 0x10, 0x10, 0xFF);
	SDL_RenderClear(renderer);

	Map::draw();

	SDL_RenderPresent(renderer);
}

void Thunder::quit()
{
	Net::close();
	Map::saveCache();

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	SDLNet_Quit();
	IMG_Quit();
	SDL_Quit();
}