#include "application.h"

int main()
{
	auto tw = new tileworld(80, 45, 32);
	auto src = new source(tw);
	application app(tw, src, "Raycasting");
	app.run();
	delete tw;
	delete src;
}
