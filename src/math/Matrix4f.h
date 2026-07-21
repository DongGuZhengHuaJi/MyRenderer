#pragma once
#include <cmath>

#include "Vector3f.h"
#include "Vector4f.h"

class Matrix4f
{
public:

    float m[16]; // 列优先

    Matrix4f() {
        setToIdentity();
    }

    void setToIdentity() {
        for (int i = 0; i < 16; ++i) {
            m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        }
    }

    Matrix4f operator+(const Matrix4f& other) const {
        Matrix4f result;
        for (int i = 0; i < 16; ++i) {
            result.m[i] = m[i] + other.m[i];
        }
        return result;
    }

    Matrix4f operator-(const Matrix4f& other) const {
        Matrix4f result;
        for (int i = 0; i < 16; ++i) {
            result.m[i] = m[i] - other.m[i];
        }
        return result;
    }

    Matrix4f operator*(const Matrix4f& other) const {
        Matrix4f result;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                result.m[col * 4 + row] = 
                    m[0 * 4 + row] * other.m[col * 4 + 0] +
                    m[1 * 4 + row] * other.m[col * 4 + 1] +
                    m[2 * 4 + row] * other.m[col * 4 + 2] +
                    m[3 * 4 + row] * other.m[col * 4 + 3];
            }
        }
        return result;
    }

    Vector3f operator*(Vector3f vec) const {
        Vector3f result;
        result.x = m[0] * vec.x + m[4] * vec.y + m[8] * vec.z + m[12];
        result.y = m[1] * vec.x + m[5] * vec.y + m[9] * vec.z + m[13];
        result.z = m[2] * vec.x + m[6] * vec.y + m[10] * vec.z + m[14];
        return result;
    }

    Vector4f operator*(Vector4f vec) const {
        Vector4f result;
        result.x = m[0] * vec.x + m[4] * vec.y + m[8] * vec.z + m[12] * vec.w;
        result.y = m[1] * vec.x + m[5] * vec.y + m[9] * vec.z + m[13] * vec.w;
        result.z = m[2] * vec.x + m[6] * vec.y + m[10] * vec.z + m[14] * vec.w;
        result.w = m[3] * vec.x + m[7] * vec.y + m[11] * vec.z + m[15] * vec.w;
        return result;
    }

    float& operator()(int row, int col) {
        return m[col * 4 + row];
    }

    float operator()(int row, int col) const {
        return m[col * 4 + row];
    }

    Matrix4f transpose() const {
        Matrix4f result;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                result(row, col) = (*this)(col, row);
            }
        }
        return result;
    }


};