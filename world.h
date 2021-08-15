#pragma once
#include <vector>
#include <algorithm>
#include "geometry.h"

class world
{
	struct edge
	{
		vec2 begin = { 0.f, 0.f };
		vec2 end = { 0.f, 0.f };
	};

	struct tile
	{
		bool solid = false;
		bool edge_exist[4] = { false };
		int edge_id[4] = { 0 };
	};

	struct ray
	{
		vec2 pos = { 0.f, 0.f };
		float angle = 0.f;
	};

	enum sides
	{
		N = 0,
		E = 1,
		S = 2,
		W = 3
	};

	std::vector<tile*> _grid;
	std::vector<edge*> _map;
	std::vector<ray*> _rays;

	uint32_t _width;
	uint32_t _height;
	uint32_t _size;
	uint32_t _dim;

	tile*& get(uint32_t x, uint32_t y);
	tile* get(uint32_t x, uint32_t y) const;

	void free_grid();
	void free_map();
	void free_rays();

	void clear_edge_data();
	std::vector<tile*> get_neighbors(uint32_t x, uint32_t y);
	void handle_edge(uint32_t x, uint32_t y, sides side, tile* neighbor, tile* borrow_from);

	void apply_geometry();
	void cast_rays(const vec2& source);

public:
	world(uint32_t width, uint32_t height, uint32_t dimension);
	~world();


};
