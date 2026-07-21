#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include "scene/Camera.h"
#include "render/Renderer.h"


class RenderViewport : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    explicit RenderViewport(QWidget *parent = nullptr);
    ~RenderViewport() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void initShaders();
    void initGeometry();

private:
    QTimer* m_timer;                        // 驱动帧率刷新的定时器

    Renderer* m_renderer;                   // 渲染器对象

    Camera m_camera;                        // 摄像机对象

    Model m_model;                          // 模型对象
    
    Shader m_shader;                        // 着色器对象

    QPoint m_lastMousePos; // 记录上一次鼠标位置

    bool m_firstMousePress = true; // 标记是否是第一次鼠标按下事件

    float m_cameraSpeed = 0.1f; // 摄像机移动速度

    float m_sensitivity = 0.05f; // 鼠标灵敏度

    float m_rotationAngle = 0.0f; // 立方体旋转角度

};