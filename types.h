#pragma once

struct edge
{
	vec2f begin;
	vec2f end;
};

struct tile
{
	bool solid = false;
	bool edge_exist[4] = { false };
	int edge_id[4] = { -1 };
};

struct ray
{
	vec2f pos;
	float angle = 0.f;
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
};

enum sides
{
	N = 0,
	E = 1,
	S = 2,
	W = 3
};
