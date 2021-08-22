#include "application.h"

int main()
{
	auto tw = new tileworld(160, 90, 20);
	auto src = new source(tw);
	application app(tw, src, "Raycasting");
	app.run();
	delete tw;
	delete src;
}
