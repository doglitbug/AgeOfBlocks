#include <SDL3/SDL_rect.h>

#include "Camera.h"
#include <iostream>
#include <algorithm>

Camera::Camera()
{
    m_position = glm::vec3(4.0f, 1.5f, 11.0f);
    m_direction = glm::vec3(0.0f, 0.0f, -1.0f);//Not important for starting value
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    pitch = 0.0f;
    yaw = -90.0f; // Looking down Z axis
}

void Camera::setPerspective(const float fov, const int screenWidth, const int screenHeight, const float near, const float far)
{
    const float aspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
    m_perspective = glm::perspectiveLH(glm::radians(fov), aspect, near, far);
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAtLH(m_position, m_position + m_direction, m_up);
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return m_perspective;
}

void Camera::move(const glm::vec2 movement)
{
    // Lets do forward/backward movement. Ignore the up/down so that we walk along the ground (instead of flying)
    // TODO Dont ignore if flying
    m_position += glm::vec3(m_direction.x, m_direction.y, m_direction.z) * movement.y;

    // Let's do strafing!
    const glm::vec3 rightVector = glm::cross(m_direction, m_up);
    m_position -= rightVector * movement.x;
}

void Camera::mouseLook(glm::vec2 look)
{
    //TODO this is kinda jerky at this speed, need to fix smoothness
    yaw -= look.x * 50.0f;// TODO Sensitivity setting for both axis
    pitch += look.y * 50.0f;// TODO invert Y setting here

    // Clamp pitch to prevent flipping
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    m_direction = glm::normalize(front);
}
