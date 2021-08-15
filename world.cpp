#include "world.h"

world::tile*& world::get(uint32_t x, uint32_t y)
{
	return _grid[y * _width + x];
}

world::tile* world::get(uint32_t x, uint32_t y) const
{
	return _grid[y * _width + x];
}

void world::free_grid()
{
	for (auto& tile : _grid)
		delete tile;
	_grid.clear();
}

void world::free_map()
{
	for (auto& edge : _map)
		delete edge;
	_map.clear();
}

void world::free_rays()
{
	for (auto& ray : _rays)
		delete ray;
	_rays.clear();
}

void world::clear_edge_data()
{
	for (auto& tile : _grid)
	{
		for (int i = 0; i < 4; i++)
		{
			tile->edge_exist[i] = false;
			tile->edge_id[i] = 0;
		}
	}
}

std::vector<world::tile*> world::get_neighbors(uint32_t x, uint32_t y)
{
	std::vector<tile*> neighbors(4);
	neighbors[N] = get(x, y - 1);
	neighbors[E] = get(x + 1, y);
	neighbors[S] = get(x, y + 1);
	neighbors[W] = get(x - 1, y);
	return neighbors;
}

void world::handle_edge(uint32_t x, uint32_t y, sides side, tile* neighbor, tile* borrow_from)
{
	if (!neighbor->solid)
	{
		auto tile = get(x, y);

		if (borrow_from->edge_exist[side])
		{
			auto& end = _map[borrow_from->edge_id[side]]->end;
			if (side == E || side == W)
				end.y += _dim;
			else
				end.x += _dim;

			tile->edge_exist[side] = true;
			tile->edge_id[side] = borrow_from->edge_id[side];
		}
		else
		{
			auto edge = new world::edge;
			edge->begin = vec2(x * _dim, y * _dim);
			edge->end = edge->begin + vec2(_dim, _dim);

			int id = _map.size();
			_map.push_back(edge);

			tile->edge_exist[side] = true;
			tile->edge_id[side] = id;
		}
	}
}

//convert tilegrid to edgemap
void world::apply_geometry()
{
	free_map();
	clear_edge_data();

	for (int y = 0; y < _height; y++)
	{
		for (int x = 0; x < _width; x++)
		{
			auto tile = get(x, y);
			if (!tile->solid)
				continue;

			auto neighbors = get_neighbors(x, y);
			handle_edge(x, y, N, neighbors[N], neighbors[W]);
			handle_edge(x, y, E, neighbors[E], neighbors[N]);
			handle_edge(x, y, S, neighbors[S], neighbors[W]);
			handle_edge(x, y, W, neighbors[W], neighbors[N]);
		}
	}
}

//construct visibility polygon
void world::cast_rays(const vec2& source)
{

}

//public interface
world::world(uint32_t width, uint32_t height, uint32_t dimension)
	: _width(width), _height(height), _size(width * height), _dim(dimension)
{
	_grid = std::vector<tile*>(_size);
	for (auto& t : _grid)
		t = new tile;
	for (int x = 0; x < _width; x++)
		get(x, 0)->solid = get(x, _height - 1)->solid = true;
	for (int y = 0; y < _height; y++)
		get(0, y)->solid =  get(_width - 1, y)->solid = true;
}

world::~world()
{
	free_grid();
	free_map();
	free_rays();
}
