#include "Camera3D.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Camera3D::Camera3D(glm::vec3 position, const float yaw, const float pitch, const float fov, const float aspectRatio)
        : m_position(position), m_yaw(yaw), m_pitch(pitch), m_fov(fov), m_aspectRatio(aspectRatio) {
    updateVectors();
}

void Camera3D::updateAspect(const int width, const int height) {
    if (height > 0) m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

// void Camera3D::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
//     m_yaw += xOffset;
//     m_pitch += yOffset;
//
//     // Clamp pitch to prevent camera flip (gimbal lock)
//     if (constrainPitch) {
//         m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
//     }
//
//     updateVectors();
// }

void Camera3D::updateVectors() {
    // Calculate new Front vector from Euler angles
    glm::vec3 front;
    front.x = static_cast<float>(cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
    front.y = static_cast<float>(sin(glm::radians(m_pitch)));
    front.z = static_cast<float>(sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));

    m_front = glm::normalize(front);

    // Recalculate Right and Up vectors
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}

glm::mat4 Camera3D::getViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera3D::getProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, NEAR_PLANE, FAR_PLANE);
}