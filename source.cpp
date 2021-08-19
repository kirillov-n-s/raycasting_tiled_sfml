#include "source.h"

void source::init()
{
	auto dim = _world->dim();
	auto width = _world->width();
	auto height = _world->height();

	_pos = vec2f(2.5f * dim, 2.5f * dim);
	_speed = (float)(dim * width);
	_range_max = _range = sqrtf(width * width + height * height) * dim;
	_range_min = 8.f * dim;
	_fov = PI / 4.f;
	_precision = _precision_min;
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

void source::set_precision(uint8_t val)
{
	if (!val)
		return;
	_precision = ALPHA / val;
	if (_precision > _precision_min)
		_precision = _precision_min;
	if (_precision < _precision_max)
		_precision = _precision_max;
}

//cast a ray from source to destination
vec2f source::ray_cast_dda(const vec2f& dir) const
{
	auto start = _pos;
	auto ndir = norm(dir);
	auto ray_unit = vec2f(sqrt(1 + (ndir.y / ndir.x) * (ndir.y / ndir.x)), sqrt(1 + (ndir.x / ndir.y) * (ndir.x / ndir.y)));
	auto pos = vec2u(start);
	vec2f ray_len;
	vec2i step;

	if (ndir.x < 0.f)
		step.x = -1,
		ray_len.x = (start.x - pos.x) * ray_unit.x;
	else
		step.x = 1,
		ray_len.x = (pos.x + 1 - start.x) * ray_unit.x;

	if (ndir.y < 0.f)
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

	return start + ndir * dist;
}

//cast light & shadows
std::pair<std::vector<vec2f>, uint32_t> source::line_of_sight() const
{
	std::vector<vec2f> rays;
	auto corners = _world->get_corners();
	uint32_t count;

	for (const auto& corner : corners)
	{
		auto dir = norm(corner - _pos);
		float angle = atan2f(dir.y, dir.x);

		float eps = 0.0001f;
		auto rdir = vec2f(cosf(angle + eps), sinf(angle + eps));
		auto ldir = vec2f(cosf(angle - eps), sinf(angle - eps));

		rays.push_back(ray_cast_dda(dir));
		rays.push_back(ray_cast_dda(rdir));
		rays.push_back(ray_cast_dda(ldir));
	}

	count = rays.size();

	std::sort(rays.begin(), rays.end(),
		[this](const auto& lhs, const auto& rhs)
		{
			auto ldir = norm(lhs - _pos);
			auto rdir = norm(rhs - _pos);
			return atan2f(ldir.y, ldir.x) < atan2f(rdir.y, rdir.x);
		});

	auto it = std::unique(rays.begin(), rays.end(),
		[](const auto& lhs, const auto& rhs)
		{
			return fabs(lhs.x - rhs.x) < 1.f && fabs(lhs.y - rhs.y) < 1.f;
		});

	if (it != rays.end())
		rays.erase(it, rays.end());
	return std::make_pair(rays, count);
}

std::pair<std::vector<vec2f>, uint32_t> source::field_of_view(const vec2f& dir) const
{
	auto ndir = norm(dir);
	auto angle = atan2f(ndir.y, ndir.x);
	int n = ceilf(_fov / _precision);
	std::vector<vec2f> rays(n * 2);
	uint32_t count;

	for (int i = 0; i < n; i++)
	{
		auto rangle = angle + _precision * i;
		auto langle = angle - _precision * (n - i);

		auto rdir = vec2f(cosf(rangle), sinf(rangle));
		auto ldir = vec2f(cosf(langle), sinf(langle));

		rays[i] = ray_cast_dda(ldir);
		rays[n + i] = ray_cast_dda(rdir);
	}

	auto corners = _world->get_corners();
	auto it = std::remove_if(corners.begin(), corners.end(),
		[this, ndir](const auto& val)
		{
			auto vdir = norm(val - _pos);
			auto diff = acosf(dot(ndir, vdir));
			return diff > _fov;
		});
	if (it != corners.end())
		corners.erase(it, corners.end());

	for (const auto& corner : corners)
	{
		auto dir = norm(corner - _pos);
		float angle = atan2f(dir.y, dir.x);

		float eps = 0.0001f;
		auto rdir = vec2f(cosf(angle + eps), sinf(angle + eps));
		auto ldir = vec2f(cosf(angle - eps), sinf(angle - eps));

		rays.push_back(ray_cast_dda(dir));
		rays.push_back(ray_cast_dda(rdir));
		rays.push_back(ray_cast_dda(ldir));
	}

	count = rays.size();

	auto edge = vec2f(cosf(angle - _fov), sinf(angle - _fov));
	std::sort(rays.begin(), rays.end(),
		[this, edge](const auto& lhs, const auto& rhs)
		{
			auto ldir = norm(lhs - _pos);
			auto rdir = norm(rhs - _pos);
			return dot(edge, ldir) < dot(edge, rdir);
		});

	it = std::unique(rays.begin(), rays.end(),
		[](const auto& lhs, const auto& rhs)
		{
			return fabs(lhs.x - rhs.x) < 1.f && fabs(lhs.y - rhs.y) < 1.f;
		});

	if (it != rays.end())
		rays.erase(it, rays.end());

	return std::make_pair(rays, count);
}

std::pair<vec2f, uint32_t> source::closet_collision() const
{
	int n = ceilf(2.f * PI / _precision);
	std::vector<vec2f> rays(n);

	for (int i = 0; i < n; i++)
	{
		auto angle = _precision * i;
		auto dir = vec2f(cosf(angle), sinf(angle));
		rays[i] = ray_cast_dda(dir);
	}

	auto closest = *std::min_element(rays.begin(), rays.end(),
		[this](const auto& lhs, const auto& rhs)
		{
			return len_sqr(lhs - _pos) < len_sqr(rhs - _pos);
		});

	return std::make_pair(closest, n);
}
