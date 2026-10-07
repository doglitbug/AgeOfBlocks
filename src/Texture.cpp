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

    // 1. CRITICAL: Force stbi to give you 4 channels (RGBA) or 3 channels (RGB)
    // to match exactly what you pass to OpenGL. Let's use 4 channels (RGBA)
    // since leaves usually require transparency.
    unsigned char *image_data = stbi_load(m_fileName.c_str(), &width, &height, &bpp, 4);

    if (!image_data)
    {
        std::cerr << "Unable to load image data from: " << m_fileName << " - " << stbi_failure_reason() << std::endl;
        exit(-1);
    }

    glGenTextures(1, &m_textureObject);
    glBindTexture(m_textureTarget, m_textureObject);

    // 2. CRITICAL: Change unpack alignment to 1 byte.
    // This tells OpenGL to read pixels continuously without forcing a 4-byte padding row stride.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 3. CRITICAL: Match internal format and format to GL_RGBA since we forced 4 channels above
    glTexImage2D(m_textureTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

    // 4. OPTIONAL BUT HIGHLY RECOMMENDED: Generate mipmaps to remove distance aliasing/moiré
    glGenerateMipmap(m_textureTarget);
    glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Note: Converted to glTexParameteri since filter/wrap params are integer enums
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(m_textureTarget, 0);

    stbi_image_free(image_data);

    return true;
}

void Texture::Bind(const GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(m_textureTarget, m_textureObject);
}
