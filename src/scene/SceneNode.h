#pragma once
#include <string>
#include <memory>
#include <vector>
#include "scene/Transform.h"

class SceneNode : public std::enable_shared_from_this<SceneNode> {
public:
    explicit SceneNode(std::string name) : m_name(std::move(name)), m_transform(), m_parent(nullptr), m_children() {}
    virtual ~SceneNode() = default;

    void addChild(std::shared_ptr<SceneNode> child) {
        child->m_parent = shared_from_this();
        m_children.push_back(std::move(child));
    }

public:
    Transform m_transform;

    int id; // 节点 ID，可用于调试或其他用途
    std::string m_name;
    std::shared_ptr<SceneNode> m_parent;
    std::vector<std::shared_ptr<SceneNode>> m_children;

    bool isSelected = false; // 节点是否被选中
};