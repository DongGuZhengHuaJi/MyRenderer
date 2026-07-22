#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QTimer>
#include <memory>
#include "render/Renderer.h"
#include "scene/Scene.h"


class RenderViewport : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    explicit RenderViewport(QWidget *parent = nullptr);
    ~RenderViewport() override;

    Scene& getScene() { return m_scene; }
    const Scene& getScene() const { return m_scene; }

public slots:
    void loadModel(const QString& filePath);
    void removeSelectedModel(SceneNode* node);
    void selectModel(SceneNode* node);
    void setWireframe(bool enabled);
    void resetCamera();
    void setModelTransform(SceneNode* node, float px, float py, float pz,
                           float rx, float ry, float rz,
                           float sx, float sy, float sz);

signals:
    void sceneChanged();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void initShaders();

private:
    QTimer* m_timer = nullptr;
    std::unique_ptr<Renderer> m_renderer;
    Scene m_scene;
    Shader m_shader;

    QPoint m_lastMousePos;
    bool m_firstMousePress = true;
    bool m_leftMousePressed = false;
    bool m_rightMousePressed = false;
    float m_cameraSpeed = 0.1f;
    float m_sensitivity = 0.05f;
    float m_modelRotateSpeed = 2.0f;
    bool m_wireframe = false;
};
