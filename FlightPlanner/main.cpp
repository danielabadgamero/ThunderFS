#include "ThunderCore.h"

int main(int, char**)
{
	Thunder::init("");

	while (Thunder::running)
	{
		Thunder::event();
		Thunder::draw();
	}

	return 0;
}