#pragma once

#include <glad/glad.h>

#include <string>
#include <vector>

class TextureArray
{
public:
    TextureArray(const std::string &path, const std::vector<std::string>& fileNames);
    ~TextureArray() = default;

    bool Load();

    void Bind(GLenum textureUnit) const;

private:
    std::string m_path;
    std::vector<std::string> m_fileNames;

    GLuint m_textureArrayObject;
};
