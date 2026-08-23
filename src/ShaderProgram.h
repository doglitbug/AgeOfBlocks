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

    // Ambient color information
    glm::vec3 m_ambientColor=glm::vec3(0.9f, 0.95f, 1.0f);
    float m_ambientIntensity=0.6f;
    GLint m_ambientColorLocation{};
    GLint m_ambientIntensityLocation{};

    // Diffuse color information
    glm::vec3 m_lightPosition=glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 m_lightColor=glm::vec3(1.0f, 1.0f, 1.0f);
    GLint m_lightPositionLocation{};
    GLint m_lightColorLocation{};

};
