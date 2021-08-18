#include "source.h"

void source::init()
{
	auto dim = _world->dim();
	auto width = _world->width();
	auto height = _world->height();

	_pos = vec2f(2.5f * dim, 2.5f * dim);
	_speed = (float)(dim * width);
	_range_max = _range = sqrtf(width * width + height * height) * dim;
	_range_min = 3.f * dim;
	_fov = PI / 4.f;
}

source::source(world* world)
	: _world(world)
{
	init();
}

source::source(world* world, const vec2f& pos)
	: _world(world)
{
	init();
	_pos = pos;
}

vec2f source::get_pos() const
{
	return _pos;
}

void source::move(const vec2f& dir, float elapsed)
{
	auto ndir = norm(dir);
	auto unit = ndir * _speed * elapsed;
	auto ray = ray_cast_dda(dir) - _pos;
	if (len_sqr(ray) <= 1.f)
		return;
	_pos += len(unit) < len(ray) - 1.f ? unit : ray - ndir;
}

void source::mod_range(float val)
{
	_range += val;
	if (_range < _range_min)
		_range = _range_min;
	else if (_range > _range_max)
		_range = _range_max;
}

void source::mod_fov(float val)
{
	_fov += val;
	if (_fov < _fov_min)
		_fov = _fov_min;
	else if (_fov > _fov_max)
		_fov = _fov_max;
}

//cast a ray from source to destination
vec2f source::ray_cast_dda(vec2f dir) const
{
	auto start = _pos;
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
	while (!intersect && dist < _range)
	{
		if (ray_len.x < ray_len.y)
			pos.x += step.x,
			dist = ray_len.x,
			ray_len.x += ray_unit.x;
		else
			pos.y += step.y,
			dist = ray_len.y,
			ray_len.y += ray_unit.y;

		auto tile = pos / _world->dim();
		intersect = _world->in_bounds(tile.x, tile.y) && _world->tile_solid(tile.x, tile.y);
	}

	return start + dir * dist;
}

//cast light & shadows
std::vector<vec2f> source::line_of_sight() const
{
	std::vector<vec2f> dests;
	std::vector<vec2f> corners = _world->get_corners();

	for (const auto& corner : corners)
	{
		auto dir = norm(corner - _pos);
		float angle = atan2f(dir.y, dir.x);

		float eps = 0.0001f;
		auto dir_plus = vec2f(cosf(angle + eps), sinf(angle + eps));
		auto dir_minus = vec2f(cosf(angle - eps), sinf(angle - eps));

		dests.push_back(ray_cast_dda(dir));
		dests.push_back(ray_cast_dda(dir_plus));
		dests.push_back(ray_cast_dda(dir_minus));
	}

	std::sort(dests.begin(), dests.end(),
		[this](const auto& lhs, const auto& rhs)
		{
			auto ldir = norm(lhs - _pos);
			auto rdir = norm(rhs - _pos);
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

std::vector<vec2f> source::field_of_view() const
{
	return std::vector<vec2f>();
}
