#pragma once
#include "math/Matrix4f.h"
#include "math/MathUtils.h"

class Transform{
public:

    Vector3f position;

    Vector3f rotation;

    Vector3f scale;

    Transform() : position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f) {
        
    }

    Matrix4f getTranlationMatrix() {
        return Transform::translation(position.x, position.y, position.z);
    }

    Matrix4f getScalingMatrix() {
        return Transform::scaling(scale.x, scale.y, scale.z);
    }

    Matrix4f getRotationMatrix() {
        Matrix4f rotationXMatrix = Transform::rotationX(rotation.x);
        Matrix4f rotationYMatrix = Transform::rotationY(rotation.y);
        Matrix4f rotationZMatrix = Transform::rotationZ(rotation.z);

        return rotationZMatrix * rotationYMatrix * rotationXMatrix;
    }

    Matrix4f getModelMatrix() {
        Matrix4f translationMatrix = Transform::translation(position.x, position.y, position.z);
        Matrix4f rotationXMatrix = Transform::rotationX(rotation.x);
        Matrix4f rotationYMatrix = Transform::rotationY(rotation.y);
        Matrix4f rotationZMatrix = Transform::rotationZ(rotation.z);
        Matrix4f scalingMatrix = Transform::scaling(scale.x, scale.y, scale.z);

        return translationMatrix * rotationZMatrix * rotationYMatrix * rotationXMatrix * scalingMatrix;
    }

private:

    // translation matrix
    static Matrix4f translation(float x, float y, float z) {
        Matrix4f result;
        result.m[12] = x;
        result.m[13] = y;
        result.m[14] = z;
        return result;
    };

    // scaling matrix
    static Matrix4f scaling(float x, float y, float z) {
        Matrix4f result;
        result.m[0] = x;
        result.m[5] = y;
        result.m[10] = z;
        return result;
    };

    // rotation matrices
    static Matrix4f rotationX(float angle) {
        float rad = radians(angle);
        Matrix4f result;
        result.m[5] = cos(rad);
        result.m[6] = sin(rad);
        result.m[9] = -sin(rad);
        result.m[10] = cos(rad);
        return result;
    };

    static Matrix4f rotationY(float angle) {
        float rad = radians(angle);
        Matrix4f result;
        result.m[0] = cos(rad);
        result.m[2] = -sin(rad);
        result.m[8] = sin(rad);
        result.m[10] = cos(rad);
        return result;
    };

    static Matrix4f rotationZ(float angle) {
        float rad = radians(angle);
        Matrix4f result;
        result.m[0] = cos(rad);
        result.m[1] = sin(rad);
        result.m[4] = -sin(rad);
        result.m[5] = cos(rad);
        return result;
    };
};