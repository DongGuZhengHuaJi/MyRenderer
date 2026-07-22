#pragma once
#include "scene/Camera.h"
#include "shader/Shader.h"
#include "render/Model.h"


class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;
    void render(Model& model, Camera& camera, Shader& shader) {
        // 1. 清空屏幕缓冲区和深度缓冲区
        glClearColor(0.1f, 0.15f, 0.2f, 1.0f); // 优雅的深蓝色背景
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. 激活着色器程序
        shader.bind();

        // 3. 设置 MVP 矩阵
        Matrix4f mvpMatrix = camera.getProjectionMatrix() * camera.getViewMatrix() * model.m_transform.getModelMatrix();
        shader.setMatrix4("MVP", mvpMatrix);

        // 4. 绘制模型（内部会绑定每个 mesh 的材质）
        model.draw(shader);

        // 5. 解绑着色器程序
        shader.release();

    }

};