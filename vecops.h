#pragma once
#include <cmath>
#include <SFML/System/Vector2.hpp>
#include "sgn.h"

using vec2f = sf::Vector2f;
using vec2i = sf::Vector2i;
using vec2u = sf::Vector2u;

inline vec2f operator*(const vec2f& lhs, const vec2f& rhs)
{
	return vec2f(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline vec2f& operator*=(vec2f& lhs, const vec2f& rhs)
{
	return lhs = lhs * rhs;
}

inline float dot(const vec2f& a, const vec2f& b)
{
	return a.x * b.x + a.y * b.y;
}

inline float len_sqr(const vec2f& v)
{
	return dot(v, v);
}

inline float len(const vec2f& v)
{
	return sqrt(len_sqr(v));
}

inline vec2f norm(const vec2f& v)
{
	return v / len(v);
}

inline float dist(const vec2f& a, const vec2f& b)
{
	return len(b - a);
}

inline vec2f sgn(const vec2f& v)
{
	return vec2f(sgn(v.x), sgn(v.y));
}

inline vec2f abs(const vec2f& v)
{
	return vec2f(abs(v.x), abs(v.y));
}
