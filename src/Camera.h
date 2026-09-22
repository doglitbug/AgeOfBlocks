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
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    /// @brief Move the camera along the x/y axis.
    ///        Of course, if we were a RTS camera, it would be x/z axis!
    ///        Will need to account for rotation?
    ///        in future, this cameras position would be set by the object the camera is attached to and that object would deal with collisions
    /// @param movement Movement vector
    void move(glm::vec2 movement);

    /// @brief Look around using mouse delta.
    ///        This would probably one of the thumbsticks on a gamepad.
    /// @param look Mouse movement delta
    void mouseLook(glm::vec2 look);

    //TODO Move this back to private once we follow player position
    glm::vec3 m_position{};

private:
    glm::vec3 m_up{};
    glm::vec3 m_direction{};
    glm::mat4x4 m_perspective{};

    float pitch;
    float yaw;
};
