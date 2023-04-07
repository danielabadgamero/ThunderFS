#include <string>

#include <SDL_net.h>

#include "ThunderNet.h"

void Thunder::Net::connect(std::string host)
{
	SDLNet_ResolveHost(&ip, host.c_str(), 80);
	socket = SDLNet_TCP_Open(&ip);
	socketSet = SDLNet_AllocSocketSet(1);
	SDLNet_TCP_AddSocket(socketSet, socket);
}

void Thunder::Net::send(std::string URL)
{
	std::string request{ "GET " + URL };
	SDLNet_TCP_Send(socket, request.c_str(), request.size() + 1);
}

std::string Thunder::Net::receive()
{
	std::string data{};
	if (SDLNet_CheckSockets(socketSet, 0) == 1)
		SDLNet_TCP_Recv(socket, data.data(), data.npos);
}

void Thunder::Net::close()
{
	SDLNet_TCP_DelSocket(socketSet, socket);
	SDLNet_FreeSocketSet(socketSet);
	SDLNet_TCP_Close(socket);
}