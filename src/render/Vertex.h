#include "math/Vector2f.h"
#include "math/Vector3f.h"


class Vertex {
public:
    Vertex() = default;
    Vertex(const Vector3f& position) : m_position(position) {}

    Vector3f m_position;
    Vector3f m_normal;
    Vector2f m_texCoords;
};