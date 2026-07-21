#pragma once
#include "math/Vector2f.h"
#include "math/Vector3f.h"


class Vertex {
public:
    Vertex() = default;
    Vertex(const Vector3f& position) : m_position(position) {}
    Vertex(const Vector3f& position, const Vector3f& normal) : m_position(position), m_normal(normal) {}
    Vertex(const Vector3f& position, const Vector3f& normal, const Vector2f& texCoords)
        : m_position(position), m_normal(normal), m_texCoords(texCoords) {}

    Vector3f m_position;
    Vector3f m_normal;
    Vector2f m_texCoords;
};