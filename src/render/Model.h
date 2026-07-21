#pragma once 

#include "render/Mesh.h"
#include "scene/Transform.h"
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

    void draw() {
        for (const auto& mesh : m_meshes) {
            mesh->draw();
        }
    }
};