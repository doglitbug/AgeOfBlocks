#pragma once

#include <glad/glad.h>

#include <string>
#include <iostream>

class Texture
{
public:
    Texture(GLenum textureTarget, const std::string &fileName);
    ~Texture() = default;

    bool Load();

    void Bind(GLenum textureUnit) const;

private:
    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObject;
};