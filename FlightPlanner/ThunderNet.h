#ifndef THUNDER_NET
#define THUNDER_NET

#include <SDL_net.h>

namespace Thunder::Net
{
	inline TCPsocket client{};
	inline IPaddress ip{};
}

#endif