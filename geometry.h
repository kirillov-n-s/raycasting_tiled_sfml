#pragma once
#include <SFML/System/Vector2.hpp>
#include <math.h>

const float EPSILON = 0.125f;
const float PI = 3.14159265f;

//generic sign function
template <typename T>
inline int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

//2d vector operations
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

inline float dot(const vec2f& lhs, const vec2f& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

inline float len_sqr(const vec2f& vec)
{
	return dot(vec, vec);
}

inline float len(const vec2f& vec)
{
	return sqrtf(len_sqr(vec));
}

inline vec2f sgn(const vec2f& vec)
{
	return vec2f(sgn(vec.x), sgn(vec.y));
}

inline vec2f path(const vec2f& a, const vec2f& b)
{
	return b - a;
}

inline float dist(const vec2f& a, const vec2f& b)
{
	return len(path(a, b));
}

inline vec2f norm(const vec2f& vec)
{
	return vec / len(vec);
}

inline vec2f abs(const vec2f& vec)
{
	return vec2f(fabs(vec.x), fabs(vec.y));
}
