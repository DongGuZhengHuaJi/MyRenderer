#pragma once

#include <GL/gl.h>
#include <memory>
#include <qopenglfunctions_4_5_core.h>
#include <vector>

#include "render/Material.h"
#include "render/Vertex.h"
#include <QOpenGLFunctions_4_5_Core>
#include "render/Material.h"

class Mesh{
public:
    Mesh() = delete;
    Mesh(QOpenGLFunctions_4_5_Core* glFunctions, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::shared_ptr<Material>& material = nullptr) : m_glFunctions(glFunctions), m_vertices(vertices), m_indices(indices), m_material(material) {
        setupMesh();
    }

    const std::vector<Vertex>& getVertices() const { return m_vertices; }
    void setVertices(const std::vector<Vertex>& vertices) { m_vertices = vertices; }

    const std::vector<unsigned int>& getIndices() const { return m_indices; }
    void setIndices(const std::vector<unsigned int>& indices) { m_indices = indices; }

    const std::shared_ptr<Material>& getMaterial() const { return m_material; }
    void setMaterial(const std::shared_ptr<Material>& material) { m_material = material; }


    void draw() const {
        m_glFunctions->glBindVertexArray(m_vao);
        m_glFunctions->glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        m_glFunctions->glBindVertexArray(0);
    }

private:

    void setupMesh() {
        m_glFunctions->glGenVertexArrays(1, &m_vao);
        m_glFunctions->glGenBuffers(1, &m_vbo);
        m_glFunctions->glGenBuffers(1, &m_ebo);

        m_glFunctions->glBindVertexArray(m_vao);
        m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
        m_glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        m_glFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

        m_glFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_position));
        m_glFunctions->glEnableVertexAttribArray(0);
        m_glFunctions->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
        m_glFunctions->glEnableVertexAttribArray(1);
        m_glFunctions->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_texCoords));
        m_glFunctions->glEnableVertexAttribArray(2);   
    }
private:

    QOpenGLFunctions_4_5_Core* m_glFunctions;

    std::vector<Vertex> m_vertices;

    std::vector<unsigned int> m_indices;

    std::shared_ptr<Material> m_material;

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
};