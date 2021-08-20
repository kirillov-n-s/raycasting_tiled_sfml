#pragma once

struct tile
{
	bool solid = false;
	bool visible = false;
};

enum directions
{
	N,
	E,
	S,
	W,

	NW,
	NE,
	SE,
	SW
};

enum mode
{
	none,

	light,
	light_rays,

	fov,
	fov_rays,

	around
};
