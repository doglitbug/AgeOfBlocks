#include "ShaderProgram.h"

#include <cstring>
#include <bits/stl_algo.h>
#include <SDL3/SDL_log.h>

#include "glm/gtc/type_ptr.hpp"

void ShaderProgram::init()
{
    // This is not in the constructor as we need to wait until GL is initialized!
    m_shaderProgram = glCreateProgram();

    if (!m_shaderProgram)
    {
        SDL_Log("Error creating shader program");
        exit(1);
    }
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_shaderProgram);
}

void ShaderProgram::addShader(const GLenum shaderType, const char* shaderSource)
{
    const GLint shaderObj = glCreateShader(shaderType);

    if (!shaderObj)
    {
        SDL_Log("Error creating shader type %d", shaderType);
        exit(1);
    }

    const GLchar *p[1];
    p[0] = shaderSource;
    GLint lengths[1];
    lengths[0] = static_cast<GLint>(std::strlen(shaderSource));

    glShaderSource(shaderObj, 1, p, lengths);
    glCompileShader(shaderObj);

    GLint success;
    glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shaderObj, 1024, nullptr, infoLog);
        SDL_Log("Error compiling shader type %d: '%s'", shaderType, infoLog);
        exit(1);
    }

    m_shaders.push_back(shaderObj);
    glAttachShader(m_shaderProgram, shaderObj);
}

void ShaderProgram::finalise()
{
    glLinkProgram(m_shaderProgram);

    GLint success = 0;
    GLchar errorLog[1024] = {0};

    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_shaderProgram, sizeof(errorLog), nullptr, errorLog);
        SDL_Log("Error linking shader program: '%s'\n", errorLog);
        exit(1);
    }

    glValidateProgram(m_shaderProgram);
    glGetProgramiv(m_shaderProgram, GL_VALIDATE_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_shaderProgram, sizeof(errorLog), nullptr, errorLog);
        SDL_Log("Invalid shader program: '%s'\n", errorLog);
        exit(1);
    }

    for (const auto shader : m_shaders)
    {
        glDeleteShader(shader);
    }

    // Get locations for base colors
    m_ambientColorLocation = getUniformLocation("ambientColor");
    m_ambientIntensityLocation = getUniformLocation("ambientIntensity");
    m_lightPositionLocation = getUniformLocation("lightPosition");
    m_lightColorLocation = getUniformLocation("lightColor");

    for (unsigned int i = 0; i < std::size(m_boneLocation); ++i) {
        std::string name = "gBones[" + std::to_string(i) + "]";
        m_boneLocation[i] = getUniformLocation(name.c_str());
    }
}

void ShaderProgram::setAmbientColor(const glm::vec3& ambientColor)
{
    m_ambientColor = ambientColor;

}

void ShaderProgram::setAmbientIntensity(const float ambientIntensity)
{
    m_ambientIntensity = ambientIntensity;
}

GLint ShaderProgram::getUniformLocation(const std::string& uniformName) const
{
    const auto location = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
    if (location == -1)
    {
        SDL_Log("Uniform %s not found", uniformName.c_str());
        exit(1);
    }

    return location;
}

void ShaderProgram::enable() const
{
    glUseProgram(m_shaderProgram);
    //Send our color info
    //TODO Move these to the World/App, instead of a shader program
    //Ambient
    glUniform3fv(m_ambientColorLocation, 1, glm::value_ptr(m_ambientColor));
    glUniform1f(m_ambientIntensityLocation, m_ambientIntensity);
    //Diffuse
    glUniform3fv(m_lightPositionLocation, 1, glm::value_ptr(m_lightPosition));
    glUniform3fv(m_lightColorLocation, 1, glm::value_ptr(m_lightColor));
}

void ShaderProgram::SetBoneTransform(const unsigned int index, const glm::mat4& transform) const
{
    // Ensure index is within valid bounds of the array
    if (index >= std::size(m_boneLocation)) {
        return;
    }

    // Upload the matrix using the standard GLM value pointer
    glUniformMatrix4fv(m_boneLocation[index], 1, GL_FALSE, glm::value_ptr(transform));
}