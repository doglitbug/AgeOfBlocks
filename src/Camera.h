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

    /// @brief Move the camera along the x/y axis.
    ///        Of course, if we were a RTS camera, it would be x/z axis!
    ///        Will need to account for rotation?
    ///        in future, this cameras position would be set by the object the camera is attached to and that object would deal with collisions
    /// @param movement Movement vector
    void Move(glm::vec2 movement);

    /// @brief Look around using mouse delta.
    ///        This would probably one of the thumbsticks on a gamepad.
    /// @param look Mouse movement delta
    void MouseLook(glm::vec2 look);

    private:
    glm::vec3 mUp{};
    glm::vec3 mPosition{};
    glm::vec3 mDirection{};
    glm::mat4x4 mPerspective{};

    float pitch;
    float yaw;
};