#include <string>
#include <vector>

#include <SDL_net.h>

#include "ThunderNet.h"

void Thunder::Net::connect(std::string h)
{
	host = h;
	SDLNet_ResolveHost(&ip, host.c_str(), 80);
	socket = SDLNet_TCP_Open(&ip);
}

void Thunder::Net::send(std::string URL)
{
	std::string request{ "GET " + URL + " HTTP/1.0\n" + "Host: " + host + "\nUser-Agent: ThunderFlightPlanner/0.1\n\n" };
	SDLNet_TCP_Send(socket, request.c_str(), static_cast<int>(request.size()) + 1);
}

std::vector<char> Thunder::Net::receive()
{
	char buffer[10000]{};
	SDLNet_TCP_Recv(socket, buffer, 10000);
	std::vector<char> content{};
	for (int i{}; i != 10000; i++)
		content.push_back(buffer[i]);
	return content;
}

void Thunder::Net::close()
{
	SDLNet_TCP_Close(socket);
}