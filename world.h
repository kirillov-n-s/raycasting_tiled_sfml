#pragma once
#include <vector>
#include <algorithm>
#include "geometry.h"
#include "utils.h"

class tileworld
{
	struct tile
	{
		bool solid = false;
	};

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
	bool tile_solid(uint32_t x, uint32_t y) const;
	void toggle_tile(uint32_t x, uint32_t y);
	void clear();
};
