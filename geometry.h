#pragma once

#include <SFML/System/Vector2.hpp>
#include <math.h>

const float EPSILON = 0.125f;

//generic sign function
template <typename T>
inline int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

//2d vector operations
using vec2 = sf::Vector2f;

inline vec2 operator*(const vec2& lhs, const vec2& rhs)
{
	return vec2(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline vec2& operator*=(vec2& lhs, const vec2& rhs)
{
	return lhs = lhs * rhs;
}

inline float dot(const vec2& lhs, const vec2& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

inline float len_sqr(const vec2& vec)
{
	return dot(vec, vec);
}

inline float len(const vec2& vec)
{
	return sqrtf(len_sqr(vec));
}

inline vec2 sgn(const vec2& vec)
{
	return vec2(sgn(vec.x), sgn(vec.y));
}

inline vec2 path(const vec2& a, const vec2& b)
{
	return b - a;
}

inline float dist(const vec2& a, const vec2& b)
{
	return len(path(a, b));
}

inline vec2 norm(const vec2& vec)
{
	return vec / len(vec);
}

inline vec2 abs(const vec2& vec)
{
	return vec2(fabs(vec.x), fabs(vec.y));
}
