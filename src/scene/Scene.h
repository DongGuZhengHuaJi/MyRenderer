#pragma once
#include "scene/CameraNode.h"
#include "scene/SceneNode.h"
#include <memory>

class Scene {
public:
    Scene() : m_root(std::make_shared<SceneNode>("Root")) {
        m_root->addChild(std::make_shared<CameraNode>("MainCamera", Camera()));
    }
    ~Scene() = default;

    std::shared_ptr<SceneNode> getRoot() { return m_root; }
    const std::shared_ptr<SceneNode> getRoot() const { return m_root; }

    CameraNode* getActiveCamera() const {
        return findActiveCamera(m_root);
    }

    std::shared_ptr<SceneNode> getSelectedNode() const {
        return findSelectedNode(m_root);
    }

    void selectNode(SceneNode* node) {
        deselectAll(m_root);
        if (node) {
            node->isSelected = true;
        }
    }

    void deselectAll() {
        deselectAll(m_root);
    }

private:
    void deselectAll(const std::shared_ptr<SceneNode>& node) const {
        node->isSelected = false;
        for (auto& child : node->m_children) {
            deselectAll(child);
        }
    }

    CameraNode* findActiveCamera(const std::shared_ptr<SceneNode>& node) const {
        if (auto cameraNode = std::dynamic_pointer_cast<CameraNode>(node)) {
            if (cameraNode->isActive) {
                return cameraNode.get();
            }
        }
        for (const auto& child : node->m_children) {
            auto* active = findActiveCamera(child);
            if (active) return active;
        }
        return nullptr;
    }

    std::shared_ptr<SceneNode> findSelectedNode(const std::shared_ptr<SceneNode>& node) const {
        if (node->isSelected) {
            return node;
        }
        for (const auto& child : node->m_children) {
            auto selected = findSelectedNode(child);
            if (selected) return selected;
        }
        return nullptr;
    }

private:
    std::shared_ptr<SceneNode> m_root;
};
