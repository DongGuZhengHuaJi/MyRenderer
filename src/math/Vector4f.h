#pragma once

#include <cmath>

class Vector4f
{
public:
    float x, y, z, w;

    Vector4f() : x(0), y(0), z(0), w(0) {}
    Vector4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4f(const Vector4f& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
    Vector4f& operator=(const Vector4f& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
        }
        return *this;
    }

    Vector4f operator+(const Vector4f& other) const {
        return Vector4f(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    Vector4f operator-(const Vector4f& other) const {
        return Vector4f(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    Vector4f operator-() const {
        return Vector4f(-x, -y, -z, -w);
    }

    Vector4f operator*(float scalar) const {
        return Vector4f(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    Vector4f operator/(float scalar) const {
        return Vector4f(x / scalar, y / scalar, z / scalar, w / scalar);
    }

    float dot(const Vector4f& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    Vector4f cross(const Vector4f& other) const {
        return Vector4f(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x,
            0.0f
        );
    }

    float length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    Vector4f normalized() const {
        float len = length();
        if (len <= 1e-6) return Vector4f(0, 0, 0, 0);
        return *this / len;
    }

};