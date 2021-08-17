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

//update corners of tile-formed polygons
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

	_src_pos_base = vec2f(2.5f * _dim, 2.5f * _dim);
	_src_rad_base = _dim / 4.f;
	_src_spd_base = (float)(_dim * _width);
	_src_rng_base = sqrtf(_width * _width + _height * _height) * _dim;
	_src_fov_base = PI + 0.01f;
	_src =
	{
		_src_pos_base,
		_src_rad_base,
		_src_spd_base,
		_src_rng_base,
		_src_fov_base
	};
}

world::~world() {}

//properties
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

//tile data & manip
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

//source data & manip
vec2f world::get_src_pos() const
{
	return _src.pos;
}

float world::get_src_rad() const
{
	return _src.radius;
}

void world::move_src(const vec2f& dir, float elapsed)
{
	auto ndir = norm(dir);
	auto unit = ndir * _src.speed * elapsed;
	auto ray = ray_cast_dda(dir) - _src.pos;
	if (len_sqr(ray) <= 1.f)
		return;
	_src.pos += len(unit) < len(ray) - _src.radius ? unit : ray - ndir * _src.radius;
}

//void world::mod_src_rad(float val)
//{
//	_src.radius += val;
//	if (_src.radius < 0.f)
//		_src.radius = 0.f;
//	else if (_src.radius > _src_rad_base)
//		_src.radius = _src_rad_base;
//}
//
//void world::mod_src_rng(float val)
//{
//	_src.range += val;
//	if (_src.range < _dim)
//		_src.range = _dim;
//	else if (_src.range > _src_rng_base)
//		_src.range = _src_rng_base;
//}
//
//void world::mod_src_fov(float val)
//{
//	_src.fov += val;
//	if (_src.fov < PI * 0.25f)
//		_src.fov = PI * 0.25f;
//	else if (_src.fov > _src_fov_base)
//		_src.fov = _src_fov_base;
//}

//world features
std::vector<vec2f> world::get_corners() const
{
	return _corners;
}

//cast a ray from source to destination
vec2f world::ray_cast_dda(vec2f dir) const
{
	auto start = _src.pos;
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
	while (!intersect && dist < _src.range)
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
std::vector<vec2f> world::line_of_sight() const
{
	std::vector<vec2f> dests;

	for (const auto& corner : _corners)
	{
		auto dir = norm(corner - _src.pos);
		float angle = atan2f(dir.y, dir.x);

		float eps = 0.0001f;
		auto dir_plus = vec2f(cosf(angle + eps), sinf(angle + eps));
		auto dir_minus = vec2f(cosf(angle - eps), sinf(angle - eps));

		dests.push_back(ray_cast_dda(dir));
		dests.push_back(ray_cast_dda(dir_plus));
		dests.push_back(ray_cast_dda(dir_minus));
	}

	/*auto it = std::partition(dests.begin(), dests.end(),
		[this](const auto& val)
		{
			auto dir = norm(val - _src.pos);
			return atan2f(dir.y, dir.x) < _src.fov;
		});

	if (it != dests.end())
		dests.erase(it, dests.end());*/

	std::sort(dests.begin(), dests.end(),
		[this](const auto& lhs, const auto& rhs)
		{
			auto ldir = norm(lhs - _src.pos);
			auto rdir = norm(rhs - _src.pos);
			return atan2f(ldir.y, ldir.x) < atan2f(rdir.y, rdir.x);
		});

	auto it = std::unique(dests.begin(), dests.end(),
		[](const auto& lhs, const auto& rhs)
		{
			return fabs(lhs.x - rhs.x) < 1.f && fabs(lhs.y - rhs.y) < 1.f;
		});

	if (it != dests.end())
		dests.erase(it, dests.end());
	return dests;
}
