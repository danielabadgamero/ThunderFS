#ifndef THUNDER_NET
#define THUNDER_NET

#include <string>

#include <SDL_net.h>

namespace Thunder::Net
{
	inline TCPsocket socket{};
	inline SDLNet_SocketSet socketSet{};
	inline IPaddress ip{};

	void connect(std::string);
	void send(std::string);
	std::string receive();
	void close();
}

#endif