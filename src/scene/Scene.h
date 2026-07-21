#pragma once
#include "render/Model.h"
#include <vector>

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    Scene(std::vector<std::shared_ptr<Model>> models) : m_models(models) {}

    const std::vector<std::shared_ptr<Model>>& getModels() const {
        return m_models;
    }

    void setModels(const std::vector<std::shared_ptr<Model>>& models) {
        m_models = models;
    }

    void addModel(std::shared_ptr<Model> model) {
        m_models.push_back(std::move(model));
    }

private:
    std::vector<std::shared_ptr<Model>> m_models; // 场景中的模型列表
};