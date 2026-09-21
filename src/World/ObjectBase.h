#pragma once
#include "glad/glad.h"
#include "glm/vec3.hpp"

/**
 * Base class to represent an object in the world that can be drawn
 */
class ObjectBase
{
public:
    virtual ~ObjectBase() {};

    virtual void populateBuffers() = 0;
    virtual void render() = 0;

protected:
    glm::vec3 m_position{};
    glm::vec3 m_rotation{};
    float m_scale = 1.0f;

    GLuint m_VAO = -1;
    GLuint m_VBO = -1;
};
