#pragma once
#include "world.h"

class source
{
	world* _world;

	vec2f _pos;
	float _speed;
	float _range;
	float _fov;

	float _range_min;
	float _range_max;
	const float _fov_min = PI / 8.f;
	const float _fov_max = PI + 0.01f;

	void init();

public:
	source(world* world);
	source(world* world, const vec2f& pos);
	//source(world* world, const vec2f& pos, float spd, float rng, float fov);

	vec2f get_pos() const;
	void move(const vec2f& dir, float elapsed);
	void mod_range(float val);
	void mod_fov(float val);

	vec2f ray_cast_dda(vec2f dir) const;
	std::vector<vec2f> line_of_sight() const;
	std::vector<vec2f> field_of_view() const;
};
