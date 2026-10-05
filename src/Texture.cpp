#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Texture::Texture(const GLenum textureTarget, const std::string &fileName) : m_textureObject(0)
{
    m_textureTarget = textureTarget;
    m_fileName = fileName;
}

bool Texture::Load()
{
    if (m_fileName.empty())
    {
        std::cerr << "Filename not specified" << std::endl;
        exit(-1);
    }

    stbi_set_flip_vertically_on_load(1);
    int width = 0, height = 0, bpp = 0;

    unsigned char *image_data = stbi_load(m_fileName.c_str(), &width, &height, &bpp, 0);

    if (!image_data)
    {
        std::cerr << "Unable to load image data from: " << m_fileName << " - " << stbi_failure_reason() << std::endl;
        exit(-1);
    }

    glGenTextures(1, &m_textureObject);
    glBindTexture(m_textureTarget, m_textureObject);

    glTexImage2D(m_textureTarget, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);

    glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(m_textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(m_textureTarget, 0);

    stbi_image_free(image_data);

    return true;
}

void Texture::Bind(const GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(m_textureTarget, m_textureObject);
}
