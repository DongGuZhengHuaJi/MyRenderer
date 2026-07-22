#pragma once
#include "math/Matrix4f.h"
#include "shader/Shader.h"
#include "scene/Scene.h"
#include "scene/SceneNode.h"
#include "scene/ModelNode.h"

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;
    void renderScene(Scene& scene, Shader& shader) {
        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto* camNode = scene.getActiveCamera();
        if (!camNode) return;

        shader.bind();
        renderNode(*scene.getRoot(), *camNode, shader);
        shader.release();
    }

    void renderNode(SceneNode& node, CameraNode& camNode, Shader& shader) {
        if (auto modelNode = dynamic_cast<ModelNode*>(&node)) {
            if (modelNode->visible && modelNode->m_model && !modelNode->m_model->isEmpty()) {
                Matrix4f modelMatrix = node.m_transform.getModelMatrix();
                Matrix4f viewMatrix = camNode.getViewMatrix();
                Matrix4f projectionMatrix = camNode.getProjectionMatrix();
                Matrix4f mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
                shader.setMatrix4("MVP", mvpMatrix);
                modelNode->m_model->draw(shader);
            }
        }
        for (auto& child : node.m_children) {
            renderNode(*child, camNode, shader);
        }
    }

};