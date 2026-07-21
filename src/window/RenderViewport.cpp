#include "window/RenderViewport.h"
#include "loader/ModelLoder.h"
#include <QKeyEvent>
#include <QDebug>
#include <memory>


RenderViewport::RenderViewport(QWidget *parent)
    : QOpenGLWidget(parent),
      m_renderer(nullptr),
      m_shader(this)  // 将当前对象的 OpenGL 函数指针传递给 Shader
{
    // 强制聚焦，以便让窗口优先接收键盘事件
    setFocusPolicy(Qt::StrongFocus);

    // 启用鼠标追踪，即使没有按下鼠标按钮也能接收 mouseMoveEvent
    // setMouseTracking(true); 

    // 设置定时器，每 16ms 触发一次 update()，驱动 paintGL 循环（约 60 FPS）
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&RenderViewport::update));
    m_timer->start(16);
}

RenderViewport::~RenderViewport()
{
    // 停止定时器
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }

    // 删除渲染器对象
    if (m_renderer) {
        delete m_renderer;
        m_renderer = nullptr;
    }
}

void RenderViewport::initializeGL()
{
    // 关键：初始化当前版本的 OpenGL 函数指针
    if (!initializeOpenGLFunctions()) {
        qCritical() << "Failed to initialize OpenGL functions.";
        return;
    }

    qDebug() << "OpenGL Version:" << reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << "GPU Vendor:" << reinterpret_cast<const char*>(glGetString(GL_VENDOR));

    m_renderer = new Renderer();


    // 开启基础硬件测试
    glEnable(GL_DEPTH_TEST);

    m_camera.aspect = float(width()) / float(height());

    initShaders();
    initGeometry();
}

void RenderViewport::resizeGL(int w, int h)
{
    // 根据视口大小改变，调整显卡视口参数
    glViewport(0, 0, w, h);
    m_camera.aspect = float(w) / float(h);
}

void RenderViewport::paintGL()
{
    // 每帧递增旋转角度（约 60 FPS，每帧约 1 度）
    m_rotationAngle += 1.0f;
    if (m_rotationAngle >= 360.0f) m_rotationAngle -= 360.0f;

    for (auto& model : m_scene.getModels()) {
        model->m_transform.rotation.y = m_rotationAngle;
        m_renderer->render(*model, m_camera, m_shader);
    }
}

void RenderViewport::initShaders() {
    // 加载着色器（路径对应 CMake 复制后的目标文件夹）
    if (!m_shader.loadShaders("shaders/default.vert", "shaders/default.frag")) {
        qCritical() << "Failed to load shaders.";
    }
}

void RenderViewport::initGeometry() {
    auto model = ModelLoader::load("models/spot_triangulated.obj",
                                   static_cast<QOpenGLFunctions_4_5_Core*>(this));
    if (model) {
        m_scene.addModel(std::move(model));
    } else {
        qCritical() << "Failed to load model.";
    }
}

void RenderViewport::keyPressEvent(QKeyEvent *event) {
    // 预留：处理 WASD 摄像机位移
    switch (event->key()) {
        case Qt::Key_W:
            m_camera.position = m_camera.position + m_camera.front * m_cameraSpeed;
            break;
        case Qt::Key_S:
            m_camera.position = m_camera.position - m_camera.front * m_cameraSpeed;
            break;
        case Qt::Key_A:
            m_camera.position = m_camera.position - m_camera.right * m_cameraSpeed;
            break;
        case Qt::Key_D:
            m_camera.position = m_camera.position + m_camera.right * m_cameraSpeed;
            break;
        default:
            QOpenGLWidget::keyPressEvent(event);
            break;
        
        update();
    }
}

void RenderViewport::wheelEvent(QWheelEvent *event) {
    float delta = event->angleDelta().y() / 120.0f; // 每格滚轮为 120
    
    m_camera.position = m_camera.position - m_camera.front * m_cameraSpeed * delta;

    update();
}

void RenderViewport::mousePressEvent(QMouseEvent *event) {
    m_lastMousePos = event->pos();
    m_firstMousePress = false;
}

void RenderViewport::mouseMoveEvent(QMouseEvent *event) {
    if (m_firstMousePress) {
        m_lastMousePos = event->pos();
        m_firstMousePress = false;
        return;
    }

    QPoint currentPos = event->pos();

    float dx = float(m_lastMousePos.x() - currentPos.x()) * m_sensitivity;
    float dy = float(currentPos.y() - m_lastMousePos.y()) * m_sensitivity; // 注意 Y 轴方向

    m_lastMousePos = currentPos; // 更新鼠标位置

    m_camera.yaw += dx;
    m_camera.pitch += dy;

    if (m_camera.pitch > 89.0f) m_camera.pitch = 89.0f;
    if (m_camera.pitch < -89.0f) m_camera.pitch = -89.0f;

    m_camera.updateCameraVectors();

    update(); // 触发重绘

}