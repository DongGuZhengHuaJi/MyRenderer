#pragma once
#include "scene/SceneNode.h"
#include "scene/ModelNode.h"
#include "scene/CameraNode.h"
#include <memory>
#include <QOpenGLFunctions_4_5_Core>

enum class NodeType {
    Empty,
    Model,
    Camera,
    Light
};

class NodeFactory {
public:
    // 根据类型创建节点（gl 仅 Model 类型需要，其余忽略）
    static std::shared_ptr<SceneNode> createNode(const std::string& name,
                                                  NodeType type,
                                                  QOpenGLFunctions_4_5_Core* gl = nullptr) {
        switch (type) {
            case NodeType::Model: {
                // path 此时已作为 name 传入（即文件路径），gl 用于加载
                auto node = ModelNode::loadFromFile(name, gl);
                if (!node) return nullptr;
                return node;
            }
            case NodeType::Camera:
                return std::make_shared<CameraNode>(name, Camera());

            case NodeType::Light:
                // TODO: LightNode
                return std::make_shared<SceneNode>(name);

            case NodeType::Empty:
            default:
                return std::make_shared<SceneNode>(name);
        }
    }

    // 便捷方法：从文件路径创建 ModelNode
    static std::shared_ptr<ModelNode> createModelNode(const std::string& filePath,
                                                       QOpenGLFunctions_4_5_Core* gl) {
        return ModelNode::loadFromFile(filePath, gl);
    }

    // 便捷方法：创建 CameraNode
    static std::shared_ptr<CameraNode> createCameraNode(const std::string& name) {
        return std::make_shared<CameraNode>(name, Camera());
    }
};
