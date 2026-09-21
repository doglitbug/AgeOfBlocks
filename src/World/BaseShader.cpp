#include "BaseShader.h"

#include <cstdlib>
#include <cstring>
#include <SDL3/SDL_log.h>

BaseShader::BaseShader()
{
}

BaseShader::~BaseShader()
{
    glDeleteProgram(m_shaderProgram);
}

void BaseShader::init()
{
    m_shaderProgram = glCreateProgram();

    if (!m_shaderProgram)
    {
        SDL_Log("Error creating shader program");
        std::exit(1);
    }
}

void BaseShader::addShader(const GLenum shaderType, const char* shaderSource)
{
    const GLint shaderObj = glCreateShader(shaderType);

    if (!shaderObj)
    {
        SDL_Log("Error creating shader type %d", shaderType);
        exit(1);
    }

    const GLchar* p[1];
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

void BaseShader::finalize()
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

    m_cameraLocation = getUniformLocation("gWVP");
}

GLint BaseShader::getUniformLocation(const std::string& uniformName) const
{
    const auto location = glGetUniformLocation(m_shaderProgram, uniformName.c_str());
    if (location == -1)
    {
        SDL_Log("Uniform %s not found", uniformName.c_str());
        exit(1);
    }

    return location;
}

void BaseShader::enable()
{
    glUseProgram(m_shaderProgram);
}
