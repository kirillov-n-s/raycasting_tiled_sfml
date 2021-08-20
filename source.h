#pragma once
#include "tileworld.h"

class source
{
	tileworld* _world;

	vec2f _pos;
	float _speed;
	float _range;
	float _fov;
	float _precision;

	float _range_min;
	float _range_max;
	const float _fov_min = PI / 64.f;
	const float _fov_max = PI / 2.f;
	const float _precision_min = ALPHA;
	const float _precision_max = ALPHA / 10.f;

	void init();

public:
	source(tileworld* world);
	source(tileworld* world, const vec2f& pos);

	vec2f get_pos() const;
	float get_fov() const;
	float get_range() const;

	void move(const vec2f& dir, float elapsed);

	void mod_range(float val);
	void mod_fov(float val);
	void set_precision(uint8_t val);

	vec2f ray_cast_dda(const vec2f& dir) const;
	std::pair<std::vector<vec2f>, uint32_t> line_of_sight() const;
	std::pair<std::vector<vec2f>, uint32_t> field_of_view(const vec2f& dir) const;
	std::pair<vec2f, uint32_t> closest_collision() const;
};
