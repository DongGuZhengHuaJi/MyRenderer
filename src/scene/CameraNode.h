#pragma once
#include "scene/Camera.h"
#include "scene/SceneNode.h"
#include <memory>

class CameraNode : public SceneNode {
public:
    explicit CameraNode(std::string name, const Camera& camera)
        : SceneNode(std::move(name)), m_camera(std::make_shared<Camera>(camera)) {
        updateCameraVectors();
    }
    ~CameraNode() = default;

    // FPS 摄像机控制状态
    float yaw = -90.0f;
    float pitch = 0.0f;
    Vector3f position = Vector3f(0.0f, 0.0f, 3.0f);
    Vector3f front;
    Vector3f up;
    Vector3f right;

    bool isActive = true;
    std::shared_ptr<Camera> m_camera;

    void updateCameraVectors() {
        float yawRad = radians(yaw);
        float pitchRad = radians(pitch);

        front.x = cos(yawRad) * cos(pitchRad);
        front.y = sin(pitchRad);
        front.z = sin(yawRad) * cos(pitchRad);
        front = front.normalized();

        right = front.cross(Vector3f(0.0f, 1.0f, 0.0f)).normalized();
        up = right.cross(front).normalized();
    }

    Matrix4f getViewMatrix() const {
        return Camera::lookAt(position, position + front, up);
    }

    Matrix4f getProjectionMatrix() const {
        return m_camera->getProjectionMatrix();
    }

    void reset() {
        yaw = -90.0f;
        pitch = 0.0f;
        position = Vector3f(0.0f, 0.0f, 3.0f);
        updateCameraVectors();
        *m_camera = Camera();
    }
};
