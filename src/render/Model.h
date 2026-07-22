#pragma once

#include "render/Mesh.h"
#include "scene/Transform.h"
#include "shader/Shader.h"
#include <memory>
#include <vector>

class Model {
public:

    std::vector<std::shared_ptr<Mesh>> m_meshes;
    Transform m_transform;



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
};