#pragma once

struct tile
{
	bool solid = false;
};

struct source
{
	vec2f pos;
	float radius;
	float speed;
	float range;
};

enum dirs
{
	N = 0,
	E = 1,
	S = 2,
	W = 3,

	NW = 4,
	NE = 5,
	SE = 6,
	SW = 7
};
