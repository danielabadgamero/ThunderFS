#ifndef THUNDER_MAP
#define THUNDER_MAP

namespace Thunder::Map
{
	class Camera
	{
	private:
		int zoom{};
		int x{};
		int y{};
	public:
		int getZoom();
		int getX();
		int getY();
		void zoomIn();
		void zoomOut();
		void move(int, int);
	} camera{};
}

#endif