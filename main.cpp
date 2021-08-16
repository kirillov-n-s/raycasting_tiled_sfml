#include "application.h"

int main()
{
	auto w = new world(40, 30, 64);
	application app(w, "Raycasting");
	app.run();
	delete w;
}
