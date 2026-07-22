#pragma once
#include "scene/SceneNode.h"
#include "render/Model.h"

class ModelNode : public SceneNode {
public:
    ModelNode(std::string name, std::shared_ptr<Model> model)
        : SceneNode(std::move(name)), m_model(std::move(model)) {}
    ~ModelNode() = default;

public:
    bool visible = true; // 可见性标志
    std::shared_ptr<Model> m_model;
};