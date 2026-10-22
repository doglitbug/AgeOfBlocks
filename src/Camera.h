#pragma once

#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
public:
    Camera();
    ~Camera() = default;

    void setPerspective(float fov, int screenWidth, int screenHeight, float near, float far);
    glm::mat4 getViewMatrix(const glm::vec3& playerPosition);
    glm::mat4 getProjectionMatrix();

    void mouseLook(glm::vec2 look, float deltaTime);
    void changeDistance(float distance);
    float getYaw() { return m_yaw; };

private:
    glm::mat4x4 m_perspective{};

    glm::vec3 calculatePosition(const glm::vec3& playerPosition);

    // Distance from the player
    float m_distance;
    // Pitch (up/down)
    float m_pitch;
    // Yaw (rotation)
    float m_yaw;
};
