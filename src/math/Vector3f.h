#pragma once
#include <cmath>

class Vector3f
{
public:
    float x, y, z;

    Vector3f() : x(0), y(0), z(0) {}
    Vector3f(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3f(const Vector3f& other) : x(other.x), y(other.y), z(other.z) {}
    Vector3f& operator=(const Vector3f& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }

    Vector3f operator+(const Vector3f& other) const {
        return Vector3f(x + other.x, y + other.y, z + other.z);
    }

    Vector3f operator-(const Vector3f& other) const {
        return Vector3f(x - other.x, y - other.y, z - other.z);
    }

    Vector3f operator-() const {
        return Vector3f(-x, -y, -z);
    }

    Vector3f operator*(float scalar) const {
        return Vector3f(x * scalar, y * scalar, z * scalar);
    }

    Vector3f operator/(float scalar) const {
        return Vector3f(x / scalar, y / scalar, z / scalar);
    }

    float dot(const Vector3f& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3f cross(const Vector3f& other) const {
        return Vector3f(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3f normalized() const {
        float len = length();
        if (len <= 1e-6) return Vector3f(0, 0, 0);
        return *this / len;
    }

};