#include "application.h"

int main()
{
	auto tw = new world(40, 30, 64);
	auto src = new source(tw);
	application app(tw, src, "Raycasting");
	app.run();
	delete tw;
}
