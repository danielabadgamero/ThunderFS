#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_net.h>
#include <SDL_ttf.h>

#include "ThunderCore.h"
#include "ThunderMap.h"
#include "ThunderNet.h"

void Thunder::init(const char* title)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	SDLNet_Init();
	TTF_Init();
	SDL_GetCurrentDisplayMode(0, &monitor);

	SDL_Window* tempWindow{ SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, monitor.w / 4, monitor.h / 4, SDL_WINDOW_BORDERLESS) };
	SDL_Renderer* tempRenderer{ SDL_CreateRenderer(tempWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) };

	TTF_Font* font{ TTF_OpenFont("C:\\Windows\\Fonts\\AGENCYR.TTF", 32) };
	std::string loadingMessage{ "Loading cache" };
	std::string connectingMessage{ "Connecting to server" };
	int timeFlag1{};
	int timeFlag2{};

	load.thread = SDL_CreateThread(Map::loadCache, "load", nullptr);
	while (!load.done)
	{
		if (SDL_GetTicks() - timeFlag2 > 3000)
		{
			timeFlag2 = SDL_GetTicks();
			loadingMessage = "Loading cache";
		}
		else if (SDL_GetTicks() - timeFlag1 > 1000)
		{
			timeFlag1 = SDL_GetTicks();
			loadingMessage.push_back('.');
		}
		SDL_Event e;
		while (SDL_PollEvent(&e));

		SDL_Rect progressFrame{ 5, monitor.h / 4 - 15, monitor.w / 4 - 10, 10 };
		SDL_Rect progressBar{ 5, monitor.h / 4 - 15, static_cast<int>(Map::loadingCacheProgress * (monitor.w / 4 - 10)), 10 };

		SDL_SetRenderDrawColor(tempRenderer, 0x10, 0x10, 0x10, 0xff);
		SDL_RenderClear(tempRenderer);
		SDL_Rect textRect{ 10, monitor.h / 4 - 60 };
		SDL_Surface* textSurface{ TTF_RenderText_Blended(font, loadingMessage.c_str(), { 0xaa, 0xaa, 0xaa, 0xff }) };
		SDL_Texture* texture{ SDL_CreateTextureFromSurface(tempRenderer, textSurface) };
		SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);
		SDL_RenderCopy(tempRenderer, texture, NULL, &textRect);
		SDL_SetRenderDrawColor(tempRenderer, 0x40, 0x40, 0x40, 0xff);
		SDL_RenderFillRect(tempRenderer, &progressFrame);
		SDL_SetRenderDrawColor(tempRenderer, 0xff, 0x40, 0x40, 0xff);
		SDL_RenderFillRect(tempRenderer, &progressBar);
		SDL_RenderPresent(tempRenderer);

		SDL_FreeSurface(textSurface);
		SDL_DestroyTexture(texture);
	}
	SDL_WaitThread(load.thread, NULL);
	TTF_CloseFont(font);

	SDL_DestroyWindow(tempWindow);
	SDL_DestroyRenderer(tempRenderer);

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, monitor.w, monitor.h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	Net::connect("tile.openstreetmap.org");

	running = true;
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
			{
				if (e.motion.x == monitor.w - 1 && e.motion.xrel > 0)
					SDL_WarpMouseInWindow(window, 0, e.motion.y);

				if (e.motion.y == monitor.h - 1 && e.motion.yrel > 0)
					SDL_WarpMouseInWindow(window, e.motion.x, 0);

				if (e.motion.x == 0 && e.motion.xrel < 0)
					SDL_WarpMouseInWindow(window, monitor.w, e.motion.y);

				if (e.motion.y == 0 && e.motion.yrel < 0)
					SDL_WarpMouseInWindow(window, e.motion.x, monitor.h);

				Map::camera.move(e.motion.xrel, e.motion.yrel);
			}
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

	SDL_SetRenderDrawColor(renderer, 0x10, 0x10, 0x10, 0xFF);
	SDL_RenderClear(renderer);

	Map::updateTiles();
	Map::draw();

	SDL_RenderPresent(renderer);
}

void Thunder::quit()
{
	Net::close();
	Map::saveCache();

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	TTF_Quit();
	SDLNet_Quit();
	IMG_Quit();
	SDL_Quit();
}