#pragma once
#include <vector>
#include <algorithm>
#include "vecops.h"
#include "types.h"

class tileworld
{
	std::vector<tile> _grid;
	std::vector<vec2f> _corners;

	uint32_t _width;
	uint32_t _height;
	uint32_t _size;
	uint32_t _dim;

	tile& get(uint32_t x, uint32_t y);
	tile get(uint32_t x, uint32_t y) const;

	std::vector<tile> get_neighbors(uint32_t x, uint32_t y);
	void update_corners();

public:
	tileworld(uint32_t width, uint32_t height, uint32_t dimension);
	~tileworld();

	uint32_t width() const;
	uint32_t height() const;
	uint32_t dim() const;
	std::vector<vec2f> get_corners() const;

	bool in_bounds(uint32_t x, uint32_t y) const;
	bool is_solid(uint32_t x, uint32_t y) const;
	void toggle_solid(uint32_t x, uint32_t y);
	void clear();

	bool is_visible(uint32_t x, uint32_t y) const;
	void set_visible(uint32_t x, uint32_t y);
	void reset_visible();
};
