#include "application.h"

int main()
{
	auto w = new world(42, 22, 64);
	application app(w, "Raycasting");
	app.run();
	delete w;
}
