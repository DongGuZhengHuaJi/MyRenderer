#pragma once

#include <cmath>

#include "math/Vector3f.h"
#include "math/Matrix4f.h"
#include "math/MathUtils.h"

class Camera
{
public:
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;

    Camera() : fov(45.0f), aspect(4.0f / 3.0f), nearPlane(0.1f), farPlane(100.0f) {}

    Matrix4f getProjectionMatrix() const {
       return Camera::perspective(fov, aspect, nearPlane, farPlane);
    }

    // --- 静态工具方法 ---

    static Matrix4f lookAt(const Vector3f& eye, const Vector3f& center, const Vector3f& up) {
        Vector3f f = (center - eye).normalized();
        Vector3f s = f.cross(up).normalized();
        Vector3f u = s.cross(f);

        Matrix4f result;
        result.m[0] = s.x;   result.m[4] = s.y;   result.m[8]  = s.z;   result.m[12] = -s.dot(eye);
        result.m[1] = u.x;   result.m[5] = u.y;   result.m[9]  = u.z;   result.m[13] = -u.dot(eye);
        result.m[2] = -f.x;  result.m[6] = -f.y;  result.m[10] = -f.z;  result.m[14] = f.dot(eye);
        result.m[3] = 0.0f;  result.m[7] = 0.0f;  result.m[11] = 0.0f;  result.m[15] = 1.0f;
        return result;
    }

    static Matrix4f orthographic(float left, float right, float bottom, float top, float near, float far) {
        Matrix4f result;
        result.m[0] = 2.0f / (right - left);
        result.m[5] = 2.0f / (top - bottom);
        result.m[10] = -2.0f / (far - near);
        result.m[12] = -(right + left) / (right - left);
        result.m[13] = -(top + bottom) / (top - bottom);
        result.m[14] = -(far + near) / (far - near);
        return result;
    }

    static Matrix4f perspective(float fov, float aspect, float near, float far) {
        float tanHalfFov = tan(radians(fov) / 2.0f);
        Matrix4f result;
        result.m[0] = 1.0f / (aspect * tanHalfFov);
        result.m[5] = 1.0f / (tanHalfFov);
        result.m[10] = -(far + near) / (far - near);
        result.m[11] = -1.0f;
        result.m[14] = -(2.0f * far * near) / (far - near);
        result.m[15] = 0.0f;
        return result;
    }
};
