#pragma once
#include <vector>

#include "glad/glad.h"
#include "Utils.h"

class BaseShader
{
public:
    BaseShader();
    virtual ~BaseShader();

    virtual void init();
    virtual void populateBuffers(){};
    virtual void enable();
    virtual void finalize();
    GLint getUniformLocation(const std::string &uniformName) const;
    GLint m_cameraLocation;

protected:
    void addShader(GLenum shaderType, const char* shaderSource);

private:
    GLuint m_shaderProgram;
    std::vector<GLuint> m_shaders;
};