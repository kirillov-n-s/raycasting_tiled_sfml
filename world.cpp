#include "world.h"

bool world::in_bounds(uint32_t x, uint32_t y) const
{
	return x < _width && y < _height;
}

tile& world::get(uint32_t x, uint32_t y)
{
	return _grid[y * _width + x];
}

tile world::get(uint32_t x, uint32_t y) const
{
	return _grid[y * _width + x];
}

std::vector<tile> world::get_neighbors(uint32_t x, uint32_t y)
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

//void world::handle_edge(uint32_t x, uint32_t y, dirs side, tile* neighbor, tile* borrow_from)
//{
//	if (!neighbor->solid)
//	{
//		auto tile = get(x, y);
//
//		if (borrow_from->edge_exist[side])
//		{
//			auto& end = _corners[borrow_from->edge_id[side]]->end;
//			if (side == E || side == W)
//				end.y += _dim;
//			else
//				end.x += _dim;
//
//			tile->edge_exist[side] = true;
//			tile->edge_id[side] = borrow_from->edge_id[side];
//		}
//		else
//		{
//			auto edge = new ::edge;
//			switch (side)
//			{
//			case N:
//				edge->begin = vec2f(x * _dim, y * _dim);
//				edge->end = edge->begin + vec2f(_dim, 0.f);
//				break;
//			case E:
//				edge->begin = vec2f((x + 1) * _dim, y * _dim);
//				edge->end = edge->begin + vec2f(0.f, _dim);
//				break;
//			case S:
//				edge->begin = vec2f(x * _dim, (y + 1) * _dim);
//				edge->end = edge->begin + vec2f(_dim, 0.f);
//				break;
//			case W:
//				edge->begin = vec2f(x * _dim, y * _dim);
//				edge->end = edge->begin + vec2f(0.f, _dim);
//				break;
//			}
//
//			int id = _corners.size();
//			_corners.push_back(edge);
//
//			tile->edge_exist[side] = true;
//			tile->edge_id[side] = id;
//		}
//	}
//}

//void world::update_corners()
//{
//	free_map();
//	clear_edge_data();
//
//	for (int y = 1; y < _height - 1; y++)
//	{
//		for (int x = 1; x < _width - 1; x++)
//		{
//			if (!get(x, y)->solid)
//				continue;
//
//			auto neighbors = get_neighbors(x, y);
//			handle_edge(x, y, N, neighbors[N], neighbors[W]);
//			handle_edge(x, y, E, neighbors[E], neighbors[N]);
//			handle_edge(x, y, S, neighbors[S], neighbors[W]);
//			handle_edge(x, y, W, neighbors[W], neighbors[N]);
//		}
//	}
//}

//construct a vector of corners of tile-formed polygons
void world::update_corners()
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
world::world(uint32_t width, uint32_t height, uint32_t dimension)
	: _width(width), _height(height), _dim(dimension)
{
	_size = _width * _height;
	_grid = std::vector<tile>(_size);

	for (int x = 1; x < _width - 1; x++)
		get(x, 1).solid = get(x, _height - 2).solid = true;
	for (int y = 1; y < _height - 1; y++)
		get(1, y).solid = get(_width - 2, y).solid = true;

	update_corners();

	_source =
	{
		vec2f(3.f * _dim, 3.f * _dim),
		_dim / 2.f,
		(float)(_dim * _width),
		sqrtf(_width * _width + _height * _height) * _dim,
		PI * 2.f
	};
}

world::~world() {}

uint32_t world::width() const
{
	return _width;
}

uint32_t world::height() const
{
	return _height;
}

uint32_t world::dim() const
{
	return _dim;
}

bool world::tile_solid(uint32_t x, uint32_t y) const
{
	return get(x, y).solid;
}

void world::toggle_tile(uint32_t x, uint32_t y)
{
	get(x, y).solid ^= true;
	update_corners();
}

void world::clear()
{
	for (int y = 2; y < _height - 2; y++)
		for (int x = 2; x < _width - 2; x++)
			get(x, y).solid = false;
	update_corners();
}

std::vector<vec2f> world::corners() const
{
	return _corners;
}

//std::vector<vec2f> world::corners() const
//{
//	std::vector<vec2f> corners;
//
//	for (auto edge : _corners)
//	{
//		corners.push_back(edge->begin);
//		corners.push_back(edge->end);
//	}
//
//	std::sort(corners.begin(), corners.end(),
//		[](const auto& lhs, const auto& rhs)
//		{
//			if (lhs.x < rhs.x)
//				return true;
//			if (fabs(lhs.x - rhs.x) < EPSILON)
//				return lhs.y < rhs.y;
//			return false;
//		});
//
//	auto it = std::unique(corners.begin(), corners.end());
//	corners.erase(it, corners.end());
//	return corners;
//}

vec2f world::get_source_pos() const
{
	return _source.pos;
}

float world::get_source_rad() const
{
	return _source.radius;
}

void world::move_source(const vec2f& dir, float elapsed)
{
	_source.move(dir, elapsed);
	auto pos = vec2u(_source.pos) / _dim;
	if (get(pos.x, pos.y).solid)
		_source.move(-dir, elapsed);
}

//cast a ray from source to destination
vec2f world::ray_cast_dda(vec2f dir) const
{
	auto start = _source.pos;
	dir = norm(dir);
	auto ray_unit = vec2f(sqrt(1 + (dir.y / dir.x) * (dir.y / dir.x)), sqrt(1 + (dir.x / dir.y) * (dir.x / dir.y)));
	auto pos = vec2u(start);
	vec2f ray_len;
	vec2i step;

	if (dir.x < 0.f)
		step.x = -1,
		ray_len.x = (start.x - pos.x) * ray_unit.x;
	else
		step.x = 1,
		ray_len.x = (pos.x + 1 - start.x) * ray_unit.x;

	if (dir.y < 0.f)
		step.y = -1,
		ray_len.y = (start.y - pos.y) * ray_unit.y;
	else
		step.y = 1,
		ray_len.y = (pos.y + 1 - start.y) * ray_unit.y;

	bool intersect = false;
	float dist = 0.0f;
	while (!intersect && dist < _source.range)
	{
		if (ray_len.x < ray_len.y)
			pos.x += step.x,
			dist = ray_len.x,
			ray_len.x += ray_unit.x;
		else
			pos.y += step.y,
			dist = ray_len.y,
			ray_len.y += ray_unit.y;

		auto tile = pos / _dim;
		intersect = in_bounds(tile.x, tile.y) && get(tile.x, tile.y).solid;
	}

	return start + dir * dist;
}

//shadow casting
void world::line_of_sight()
{

}
