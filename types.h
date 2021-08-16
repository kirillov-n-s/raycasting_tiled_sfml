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
	float fov;

	void move(const vec2f& dir, float elapsed)
	{
		pos += dir * speed * elapsed;
	}

	bool contains(const vec2f& point) const
	{
		return dist(pos, point) < radius;
	}
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
