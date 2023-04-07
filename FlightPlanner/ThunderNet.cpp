#include <string>

#include <SDL_net.h>

#include "ThunderNet.h"

void Thunder::Net::HTTPrequest::send()
{

}

void Thunder::Net::connect(std::string host)
{
	SDLNet_ResolveHost(&ip, host.c_str(), 80);
	socket = SDLNet_TCP_Open(&ip);
}