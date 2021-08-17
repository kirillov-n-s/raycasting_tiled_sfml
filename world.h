#pragma once
#include <vector>
#include <algorithm>
#include "geometry.h"
#include "types.h"

class world
{
	std::vector<tile> _grid;
	std::vector<vec2f> _corners;

	vec2f _src_pos_base;
	float _src_rad_base;
	float _src_spd_base;
	float _src_rng_base;
	float _src_fov_base;
	source _src;

	uint32_t _width;
	uint32_t _height;
	uint32_t _size;
	uint32_t _dim;

	bool in_bounds(uint32_t x, uint32_t y) const;

	tile& get(uint32_t x, uint32_t y);
	tile get(uint32_t x, uint32_t y) const;

	std::vector<tile> get_neighbors(uint32_t x, uint32_t y);
	void update_corners();

public:
	world(uint32_t width, uint32_t height, uint32_t dimension);
	~world();

	uint32_t width() const;
	uint32_t height() const;
	uint32_t dim() const;

	bool tile_solid(uint32_t x, uint32_t y) const;
	void toggle_tile(uint32_t x, uint32_t y);
	void clear();

	vec2f get_src_pos() const;
	float get_src_rad() const;
	void move_src(const vec2f& dir, float elapsed);
	/*void mod_src_rad(float val);
	void mod_src_rng(float val);
	void mod_src_fov(float val);*/

	std::vector<vec2f> get_corners() const;
	vec2f ray_cast_dda(vec2f dir) const;
	std::vector<vec2f> line_of_sight() const;
};
