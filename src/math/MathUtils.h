#pragma once

#include <cmath>



constexpr float PI = 3.14159265358979323846f;


inline float radians(float degree)
{
    return degree * PI / 180.0f;
}


inline float degrees(float radian)
{
    return radian * 180.0f / PI;
}

