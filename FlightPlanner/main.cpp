#include "ThunderCore.h"

int main(int, char**)
{
	Thunder::init("Thunder VFR Flight Planner");

	while (Thunder::running)
	{
		Thunder::event();
		Thunder::draw();
	}

	Thunder::quit();

	return 0;
}