#include "ThunderCore.h"

int main(int, char**)
{
	Thunder::init("Thunder Flight Planner");

	while (Thunder::running)
	{
		Thunder::event();
		Thunder::draw();
	}

	Thunder::quit();

	return 0;
}