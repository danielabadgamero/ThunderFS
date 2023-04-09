#include <string>
#include <vector>

#include <SDL_net.h>

#include "ThunderCore.h"
#include "ThunderNet.h"

void Thunder::Net::connect(std::string h)
{
	host = h;
	SDLNet_ResolveHost(&ip, host.c_str(), 80);
}

void Thunder::Net::send(std::string URL)
{
	socket = SDLNet_TCP_Open(&ip);
	std::string request{ "GET " + URL + " HTTP/1.0\r\n" + "Host: " + host + "\r\nUser-Agent: ThunderFlightPlanner/0.1\r\n\r\n" };
	SDLNet_TCP_Send(socket, request.c_str(), static_cast<int>(request.size()) + 1);
}

std::vector<char> Thunder::Net::receive()
{
	std::vector<char> content{};
	do content.push_back(0);
	while (SDLNet_TCP_Recv(socket, &content.back(), 1) == 1);
	return content;
}

void Thunder::Net::close()
{
	SDLNet_TCP_Close(socket);
}