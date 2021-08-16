#pragma once
#include <vector>
#include <algorithm>
#include "geometry.h"
#include "types.h"

class world
{
	std::vector<tile> _grid;
	std::vector<vec2f> _corners;

	source _source;

	uint32_t _width;
	uint32_t _height;
	uint32_t _size;
	uint32_t _dim;

	bool in_bounds(uint32_t x, uint32_t y) const;

	tile& get(uint32_t x, uint32_t y);
	tile get(uint32_t x, uint32_t y) const;

	std::vector<tile> get_neighbors(uint32_t x, uint32_t y);
	//void handle_edge(uint32_t x, uint32_t y, dirs side, tile* neighbor, tile* borrow_from);

	void update_corners();

public:
	world(uint32_t width, uint32_t height, uint32_t dimension);
	~world();

	uint32_t width() const;
	uint32_t height() const;
	uint32_t dim() const;

	std::vector<vec2f> corners() const;

	bool tile_solid(uint32_t x, uint32_t y) const;
	void toggle_tile(uint32_t x, uint32_t y);
	void clear();

	vec2f get_source_pos() const;
	float get_source_rad() const;
	void move_source(const vec2f& dir, float elapsed);

	vec2f ray_cast_dda(vec2f dir) const;
	void line_of_sight();
};
