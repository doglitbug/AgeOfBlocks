#pragma once
#include <string>
#include <vector>

#include "glad/glad.h"
#include "glm/vec3.hpp"

class ShaderProgram
{
public:
    ShaderProgram()=default;
    ~ShaderProgram();
    void init();

    void addShader(GLenum shaderType, const char* shaderSource);
    void finalise();

    void setAmbientColor(const glm::vec3& ambientColor);
    void setAmbientIntensity(float ambientIntensity);

    [[nodiscard]]
    GLint getUniformLocation(const std::string &uniformName) const;
    void enable() const;
private:
    GLuint m_shaderProgram{};
    std::vector<GLuint> m_shaders;

    glm::vec3 m_ambientColor=glm::vec3(0.9f, 0.95f, 1.0f);
    float m_ambientIntensity=0.8f;

    GLint m_ambientColorLocation{};
    GLint m_ambientIntensityLocation{};
};
