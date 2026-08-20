#include <SDL3/SDL_rect.h>

#include "Camera.h"
#include <iostream>
#include <algorithm>

Camera::Camera()
{
    mPosition = glm::vec3(0.0f, 1.5f, 5.0f);
    mDirection = glm::vec3(0.0f, 0.0f, -1.0f);//Not important for starting value
    mUp = glm::vec3(0.0f, 1.0f, 0.0f);
    pitch = 0.0f;
    yaw = -90.0f; // Looking down Z axis
}

void Camera::setPerspective(const float fov, const int screenWidth, const int screenHeight, const float near, const float far)
{
    mPerspective = glm::perspectiveFov(glm::radians(fov),
                                       static_cast<float>(screenWidth),
                                       static_cast<float>(screenHeight),
                                       near,
                                       far);
}

glm::mat4 Camera::getViewMatrix() const
{
    return mPerspective * glm::lookAt(mPosition, mPosition + mDirection, mUp);
}

void Camera::Move(const glm::vec2 movement)
{
    // Lets do forward/backward movement. Ignore the up/down so that we walk along the ground (instead of flying)
    // TODO Dont ignore if flying
    mPosition -= glm::vec3(mDirection.x, 0.0f, mDirection.z) * movement.y;

    // Let's do strafing!
    const glm::vec3 rightVector = glm::cross(mDirection, mUp);
    mPosition += rightVector * movement.x;
}

void Camera::MouseLook(glm::vec2 look)
{
    // Up/down dead-zone
    if (std::abs(look.y) < 0.02f){
        look.y = 0.0f;
    }

    yaw += look.x * 10.0f;// TODO Sensitivity setting for both axis
    pitch += look.y * 10.0f;// TODO invert Y setting here

    // Clamp pitch to prevent flipping
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    mDirection = glm::normalize(front);
}
