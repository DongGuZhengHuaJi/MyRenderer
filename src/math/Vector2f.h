#pragma once
#include <cmath>

class Vector2f
{
public:
    float x, y;

    Vector2f() : x(0), y(0) {}
    Vector2f(float x, float y) : x(x), y(y) {}
    Vector2f(const Vector2f& other) : x(other.x), y(other.y) {}
    Vector2f& operator=(const Vector2f& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    Vector2f operator+(const Vector2f& other) const {
        return Vector2f(x + other.x, y + other.y);
    }

    Vector2f operator-(const Vector2f& other) const {
        return Vector2f(x - other.x, y - other.y);
    }

    Vector2f operator-() const {
        return Vector2f(-x, -y);
    }

    Vector2f operator*(float scalar) const {
        return Vector2f(x * scalar, y * scalar);
    }

    Vector2f operator/(float scalar) const {
        return Vector2f(x / scalar, y / scalar);
    }

    float dot(const Vector2f& other) const {
        return x * other.x + y * other.y;
    }

    Vector2f cross(const Vector2f& other) const {
        return Vector2f(
            y * other.x - x * other.y,
            x * other.y - y * other.x
        );
    }

    float length() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2f normalized() const {
        float len = length();
        if (len <= 1e-6) return Vector2f(0, 0);
        return *this / len;
    }

};