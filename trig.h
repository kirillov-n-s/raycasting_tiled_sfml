#pragma once

const float PI = 3.14159265f;
const float ALPHA = PI / 180.f;

inline float deg(float rad)
{
	return rad / ALPHA;
}

inline float rad(float deg)
{
	return deg * ALPHA;
}
