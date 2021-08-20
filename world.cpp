#include "world.h"

tileworld::tile& tileworld::get(uint32_t x, uint32_t y)
{
	return _grid[y * _width + x];
}

tileworld::tile tileworld::get(uint32_t x, uint32_t y) const
{
	return _grid[y * _width + x];
}

std::vector<tileworld::tile> tileworld::get_neighbors(uint32_t x, uint32_t y)
{
	std::vector<tile> neighbors(8);

	neighbors[N] = get(x, y - 1);
	neighbors[E] = get(x + 1, y);
	neighbors[S] = get(x, y + 1);
	neighbors[W] = get(x - 1, y);

	neighbors[NW] = get(x - 1, y - 1);
	neighbors[NE] = get(x + 1, y - 1);
	neighbors[SE] = get(x + 1, y + 1);
	neighbors[SW] = get(x - 1, y + 1);

	return neighbors;
}

//update corners of tile-formed polygons
void tileworld::update_corners()
{
	_corners.clear();

	for (int y = 1; y < _height - 1; y++)
	{
		for (int x = 1; x < _width - 1; x++)
		{
			if (!get(x, y).solid)
				continue;
		
			auto neighbors = get_neighbors(x, y);

			if (!(neighbors[W].solid && neighbors[NW].solid && neighbors[N].solid || (neighbors[W].solid ^ neighbors[N].solid)))
				_corners.push_back(vec2f(x * _dim, y * _dim));

			if (!(neighbors[N].solid && neighbors[NE].solid && neighbors[E].solid || (neighbors[N].solid ^ neighbors[E].solid)))
				_corners.push_back(vec2f((x + 1) * _dim, y * _dim));

			if (!(neighbors[E].solid && neighbors[SE].solid && neighbors[S].solid || (neighbors[E].solid ^ neighbors[S].solid)))
				_corners.push_back(vec2f((x + 1) * _dim, (y + 1) * _dim));

			if (!(neighbors[S].solid && neighbors[SW].solid && neighbors[W].solid || (neighbors[S].solid ^ neighbors[W].solid)))
				_corners.push_back(vec2f(x * _dim, (y + 1) * _dim));
		}
	}
}

//public interface
tileworld::tileworld(uint32_t width, uint32_t height, uint32_t dimension)
	: _width(width), _height(height), _dim(dimension)
{
	_size = _width * _height;
	_grid = std::vector<tile>(_size);

	for (int x = 1; x < _width - 1; x++)
		get(x, 1).solid = get(x, _height - 2).solid = true;
	for (int y = 1; y < _height - 1; y++)
		get(1, y).solid = get(_width - 2, y).solid = true;

	update_corners();
}

tileworld::~tileworld() {}

//properties
uint32_t tileworld::width() const
{
	return _width;
}

uint32_t tileworld::height() const
{
	return _height;
}

uint32_t tileworld::dim() const
{
	return _dim;
}

//world features
std::vector<vec2f> tileworld::get_corners() const
{
	return _corners;
}

//tile data & manip
bool tileworld::in_bounds(uint32_t x, uint32_t y) const
{
	return x < _width && y < _height;
}

bool tileworld::tile_solid(uint32_t x, uint32_t y) const
{
	return get(x, y).solid;
}

void tileworld::toggle_tile(uint32_t x, uint32_t y)
{
	get(x, y).solid ^= true;
	update_corners();
}

void tileworld::clear()
{
	for (int y = 2; y < _height - 2; y++)
		for (int x = 2; x < _width - 2; x++)
			get(x, y).solid = false;
	update_corners();
}
