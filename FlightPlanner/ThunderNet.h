#ifndef THUNDER_NET
#define THUNDER_NET

#include <string>
#include <vector>

#include <SDL_net.h>

namespace Thunder::Net
{
	inline TCPsocket socket{};
	inline IPaddress ip{};
	inline std::string host{};

	void connect(std::string);
	void send(std::string);
	std::vector<char> receive();
	void close();
}

#endif