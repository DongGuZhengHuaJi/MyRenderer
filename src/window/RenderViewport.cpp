#include "window/RenderViewport.h"
#include "loader/ModelLoder.h"
#include "scene/ModelNode.h"
#include <QKeyEvent>
#include <QDebug>
#include <QFileInfo>
#include <QWheelEvent>
#include <QMouseEvent>
#include <memory>


RenderViewport::RenderViewport(QWidget *parent)
    : QOpenGLWidget(parent),
      m_shader(this)
{
    setFocusPolicy(Qt::StrongFocus);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&RenderViewport::update));
    m_timer->start(16);
}

RenderViewport::~RenderViewport()
{
    if (m_timer) {
        m_timer->stop();
    }
}

void RenderViewport::initializeGL()
{
    if (!initializeOpenGLFunctions()) {
        qCritical() << "Failed to initialize OpenGL functions.";
        return;
    }

    qDebug() << "OpenGL Version:" << reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << "GPU Vendor:" << reinterpret_cast<const char*>(glGetString(GL_VENDOR));

    m_renderer = std::make_unique<Renderer>();

    glEnable(GL_DEPTH_TEST);

    // 设置默认摄像机的 aspect
    auto* camNode = m_scene.getActiveCamera();
    if (camNode) {
        camNode->m_camera->aspect = float(width()) / float(height());
    }

    initShaders();
}

void RenderViewport::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    auto* camNode = m_scene.getActiveCamera();
    if (camNode) {
        camNode->m_camera->aspect = float(w) / float(h);
    }
}

void RenderViewport::paintGL()
{
    m_renderer->renderScene(m_scene, m_shader);
}

void RenderViewport::initShaders() {
    if (!m_shader.loadShaders("shaders/default.vert", "shaders/default.frag")) {
        qCritical() << "Failed to load shaders.";
    }
}

void RenderViewport::keyPressEvent(QKeyEvent *event) {
    auto* camNode = m_scene.getActiveCamera();
    if (!camNode) {
        QOpenGLWidget::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
        case Qt::Key_W:
            camNode->position = camNode->position + camNode->front * m_cameraSpeed;
            break;
        case Qt::Key_S:
            camNode->position = camNode->position - camNode->front * m_cameraSpeed;
            break;
        case Qt::Key_A:
            camNode->position = camNode->position - camNode->right * m_cameraSpeed;
            break;
        case Qt::Key_D:
            camNode->position = camNode->position + camNode->right * m_cameraSpeed;
            break;
        default:
            QOpenGLWidget::keyPressEvent(event);
            break;

        update();
    }
}

void RenderViewport::wheelEvent(QWheelEvent *event) {
    auto* camNode = m_scene.getActiveCamera();
    if (!camNode) return;

    float delta = event->angleDelta().y() / 120.0f;
    camNode->position = camNode->position - camNode->front * m_cameraSpeed * delta;
    update();
}

void RenderViewport::mousePressEvent(QMouseEvent *event) {
    m_lastMousePos = event->pos();
    m_firstMousePress = false;

    if (event->button() == Qt::LeftButton) {
        m_leftMousePressed = true;
        m_rightMousePressed = false;
    } else if (event->button() == Qt::RightButton) {
        m_rightMousePressed = true;
        m_leftMousePressed = false;
    }
}

void RenderViewport::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_leftMousePressed = false;
    } else if (event->button() == Qt::RightButton) {
        m_rightMousePressed = false;
    }
}

void RenderViewport::mouseMoveEvent(QMouseEvent *event) {
    if (m_firstMousePress) {
        m_lastMousePos = event->pos();
        m_firstMousePress = false;
        return;
    }

    QPoint currentPos = event->pos();
    float dx = float(m_lastMousePos.x() - currentPos.x()) * m_sensitivity;
    float dy = float(currentPos.y() - m_lastMousePos.y()) * m_sensitivity;
    m_lastMousePos = currentPos;

    if (m_leftMousePressed) {
        // 左键：旋转摄像机
        auto* camNode = m_scene.getActiveCamera();
        if (camNode) {
            camNode->yaw += dx;
            camNode->pitch += dy;
            if (camNode->pitch > 89.0f) camNode->pitch = 89.0f;
            if (camNode->pitch < -89.0f) camNode->pitch = -89.0f;
            camNode->updateCameraVectors();
        }
    } else if (m_rightMousePressed) {
        // 右键：旋转选中的模型节点
        auto nodePtr = m_scene.getSelectedNode();
        if (nodePtr) {
            nodePtr->m_transform.rotation.y -= dx * m_modelRotateSpeed;
            nodePtr->m_transform.rotation.x += dy * m_modelRotateSpeed;
            emit sceneChanged();  // 通知 UI 更新 Inspector
        }
    }

    update();
}

// --- public slots ---

void RenderViewport::loadModel(const QString& filePath) {
    QFileInfo fi(filePath);
    if (!fi.exists()) {
        qWarning() << "File not found:" << filePath;
        return;
    }

    makeCurrent();
    auto model = ModelLoader::load(
        filePath.toStdString(),
        static_cast<QOpenGLFunctions_4_5_Core*>(this));
    doneCurrent();

    if (model) {
        m_scene.getRoot()->addChild(
            std::make_shared<ModelNode>(fi.baseName().toStdString(), model));
        emit sceneChanged();
        qDebug() << "Loaded model:" << filePath;
    } else {
        qWarning() << "Failed to load model:" << filePath;
    }
}

void RenderViewport::selectModel(SceneNode* node) {
    m_scene.selectNode(node);
    update();
}

void RenderViewport::removeSelectedModel(SceneNode* node) {
    if (!node) return;
    auto& children = m_scene.getRoot()->m_children;
    auto it = std::find_if(children.begin(), children.end(),
        [node](const std::shared_ptr<SceneNode>& child) {
            return child.get() == node;
        });
    if (it != children.end()) {
        children.erase(it);
        emit sceneChanged();
    }
}

void RenderViewport::setWireframe(bool enabled) {
    m_wireframe = enabled;
    makeCurrent();
    if (enabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    doneCurrent();
    update();
}

void RenderViewport::resetCamera() {
    auto* camNode = m_scene.getActiveCamera();
    if (!camNode) return;
    camNode->reset();
    camNode->m_camera->aspect = float(width()) / float(height());
    update();
}

void RenderViewport::setModelTransform(SceneNode* node,
                                        float px, float py, float pz,
                                        float rx, float ry, float rz,
                                        float sx, float sy, float sz) {
    if (!node) return;
    auto& t = node->m_transform;
    t.position.x = px;
    t.position.y = py;
    t.position.z = pz;
    t.rotation.x = rx;
    t.rotation.y = ry;
    t.rotation.z = rz;
    t.scale.x = sx;
    t.scale.y = sy;
    t.scale.z = sz;
    update();
}
