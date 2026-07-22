#pragma once

#include "render/Mesh.h"
#include "shader/Shader.h"
#include <memory>
#include <vector>

class Model {
public:

    std::string m_name;
    std::vector<std::shared_ptr<Mesh>> m_meshes;

    Model() = default;
    Model(const std::vector<std::shared_ptr<Mesh>>& meshes) : m_meshes(meshes) {}

    const std::vector<std::shared_ptr<Mesh>>& getMeshes() const { return m_meshes; }
    void setMeshes(const std::vector<std::shared_ptr<Mesh>>& meshes) { m_meshes = meshes; }

    void draw(Shader& shader) {
        for (const auto& mesh : m_meshes) {
            auto& material = mesh->getMaterial();
            if (material) {
                material->bind(shader);
            }
            mesh->draw();
        }
    }

    bool isEmpty() const {
        return m_meshes.empty();
    }
};