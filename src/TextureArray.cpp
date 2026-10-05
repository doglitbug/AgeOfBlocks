#include "TextureArray.h"

#include <ostream>
#include <SDL3/SDL_log.h>

#include "stb/stb_image.h"

TextureArray::TextureArray(const std::string& path, const std::vector<std::string>& fileNames)
{
    m_path = path;
    m_fileNames = fileNames;
}
bool TextureArray::Load()
{
    if (m_fileNames.empty())
    {
        SDL_Log("Filenames not specified");
        exit(-1);
    }

    stbi_set_flip_vertically_on_load(1);
    int width = 0, height = 0, bpp = 0, mipmapLevels = 1;

    //Get info from first file (we assume all will be the same size/bpp)
    unsigned char *first_image_data = stbi_load((m_path + m_fileNames[0]).c_str(), &width, &height, &bpp, 0);

    if (!first_image_data)
    {
        SDL_Log("Unable to load image data from: (%s)", m_fileNames[0].c_str());
        exit(-1);
    }
    stbi_image_free(first_image_data);

    glGenTextures(1, &m_textureArrayObject);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArrayObject);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipmapLevels, GL_RGBA8, width, height, m_fileNames.size());

    // 2. Upload individual texture layers from CPU memory
    for (int i = 0; i < m_fileNames.size(); ++i) {
        void *image_data = stbi_load((m_path + m_fileNames[i]).c_str(), &width, &height, &bpp, 4);

        // xoffset=0, yoffset=0, zoffset=i (the layer index), depth=1 (one layer at a time)
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

        stbi_image_free(image_data);
    }

    // 3. Set standard filtering parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return true;
}

void TextureArray::Bind(const GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArrayObject);
}