#ifndef THUNDER_NET
#define THUNDER_NET

#include <string>

#include <SDL_net.h>

namespace Thunder::Net
{
	class HTTPrequest
	{
	private:
		std::string host{};
	public:
		void send();
	};

	inline TCPsocket socket{};
	inline IPaddress ip{};

	void connect(std::string);
}

#endif