#include <SDL3/SDL_rect.h>

#include "Camera.h"
#include <iostream>
#include <algorithm>

Camera::Camera()
{
    m_distance = 10.0f;
    m_pitch = 0.0f;
    m_yaw = 180.0f;
}

void Camera::setPerspective(const float fov, const int screenWidth, const int screenHeight, const float near,
                            const float far)
{
    const float aspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
    m_perspective = glm::perspectiveLH(glm::radians(fov), aspect, near, far);
}

glm::mat4 Camera::getViewMatrix(const glm::vec3& playerPosition)
{
    const glm::vec3 cameraPosition = calculatePosition(playerPosition);
    const glm::vec3 target = playerPosition + glm::vec3(0.0f, 1.8f, 0.0f);
    constexpr auto up = glm::vec3(0.0f, 1.0f, 0.0f);

    return glm::lookAtLH(cameraPosition, target, up);
}

glm::mat4 Camera::getProjectionMatrix()
{
    return m_perspective;
}

void Camera::mouseLook(const glm::vec2 look, const float deltaTime)
{
    m_yaw += look.x * 100.0f * deltaTime; // TODO Sensitivity setting for both axis
    m_pitch -= look.y * 100.0f * deltaTime; // TODO invert Y setting here

    // Clamp pitch to prevent flipping
    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    m_yaw = std::fmod(m_yaw, 360.0f);
    if (m_yaw < 0.0f)
    {
        m_yaw += 360.0f;
    }
}

void Camera::changeDistance(const float distance)
{
    m_distance = std::clamp(m_distance + distance, 0.1f, 50.0f);
}

glm::vec3 Camera::calculatePosition(const glm::vec3& playerPosition)
{
    glm::vec3 camera;
    camera.x = m_distance * cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
    camera.y = m_distance * sin(glm::radians(m_pitch));
    camera.z = m_distance * cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));

    constexpr glm::vec3 eyeHeight = {0.0f, 1.8f, 0.0f};

    return playerPosition + eyeHeight + camera;
}
