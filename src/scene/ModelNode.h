#pragma once
#include "scene/SceneNode.h"
#include "loader/ModelLoder.h"
#include "render/Model.h"
#include <memory>

class ModelNode : public SceneNode {
public:
    ModelNode(std::string name, std::shared_ptr<Model> model)
        : SceneNode(std::move(name)), m_model(std::move(model)) {}
    ~ModelNode() = default;

    // 从文件加载并创建 ModelNode
    static std::shared_ptr<ModelNode> loadFromFile(const std::string& path,
                                                    QOpenGLFunctions_4_5_Core* gl) {
        auto model = ModelLoader::load(path, gl);
        if (!model) return nullptr;
        return std::make_shared<ModelNode>(model->m_name, model);
    }

public:
    bool visible = true;
    std::shared_ptr<Model> m_model;
};
