#include "map.h"

#include <cmath>

map::map(const int size)
{
    m_size = size;
    m_textureArray = {};
    loadTextures();
    m_terrainShaderProgram.init();
    glGenVertexArrays(1, &m_VAO);
    generateMap();
}

void map::loadTextures()
{
    const auto terrainPath = "assets/terrain/";
    const std::vector<std::string> filenames = {
        "RockWall_Texture_01.png",
        "Grass_Clovers_Texture_01.png",
        "Rock_Texture_01.png",
        "Sand_Texture_01.png"
    };

    m_textureArray = new TextureArray(terrainPath, filenames);
    m_textureArray->Load();
}

void map::render()
{
    m_terrainShaderProgram.enable();

    //TODO Cache these values/add to uniform across all shaders

    glUniform1i(m_terrainShaderProgram.getUniformLocation("mapSize"), m_size);
    glUniform1f(m_terrainShaderProgram.getUniformLocation("heightScale"), 1.0f);

    // Slot 0: terrain types
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTextureID);
    glUniform1i(m_terrainShaderProgram.getUniformLocation("terrainMap"), 0);

    // Slot 1: terrain heights
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, heightTextureID);
    glUniform1i(m_terrainShaderProgram.getUniformLocation("heightMap"), 1);

    // Slot 2: texture array
    m_textureArray->Bind(GL_TEXTURE2);
    glUniform1i(m_terrainShaderProgram.getUniformLocation("textureArray"), 2);
    // Draw
    glBindVertexArray(m_VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_size * m_size);

    // Clean up
    glBindVertexArray(0);
    glUseProgram(0);
}

void map::generateMap()
{
    //TODO Add back in walkable/buildable etc
    terrainMap.resize(m_size * m_size);

    for (int z = 0; z < m_size; ++z)
    {
        for (int x = 0; x < m_size; ++x)
        {
            auto terrainType = Grass;
            if (x < 2 || z < 2) { terrainType = Sand; }
            //(*this)(z, x) = cell(terrainType, true, true);
            terrainMap[z * m_size + x] = terrainType;
        }
    }

    CreateHeightMapTexture();
    CreateTerrainMapTexture();
}

void map::CreateHeightMapTexture()
{
    // 1. Calculate vertex counts (grid dimensions + 1)
    const int vWidth = m_size + 1;
    const int vHeight = m_size + 1;

    // 2. Allocate CPU array for heights
    heightMap.resize(vWidth * vHeight, 0.0f);

    // 3. Optional: Populate with sample data (e.g., creating a simple hill)
    for (int y = 0; y < vHeight; ++y)
    {
        for (int x = 0; x < vWidth; ++x)
        {
            // Generates a smooth test mound in the center of your map
            float dx = static_cast<float>(x) - (vWidth / 2.0f);
            float dy = static_cast<float>(y) - (vHeight / 2.0f);
            float dist = std::sqrt(dx * dx + dy * dy);
            heightMap[y * vWidth + x] = std::max(0.0f, 10.0f - dist * 0.5f);
        }
    }

    // 4. Generate the OpenGL Texture Object
    glGenTextures(1, &heightTextureID);
    glBindTexture(GL_TEXTURE_2D, heightTextureID);

    // 5. Use GL_LINEAR filtering so the GPU smoothly interpolates between vertices
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 6. Upload data as 32-bit Single Channel Floats (GL_R32F)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, vWidth, vHeight, 0, GL_RED, GL_FLOAT, heightMap.data());

    glBindTexture(GL_TEXTURE_2D, 0);
}

void map::CreateTerrainMapTexture()
{
    // 4. Generate the OpenGL Texture Object
    glGenTextures(1, &terrainTextureID);
    glBindTexture(GL_TEXTURE_2D, terrainTextureID);

    // 5. Use GL_LINEAR filtering so the GPU smoothly interpolates between vertices
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 6. Upload data as 32-bit Single Channel Floats (GL_R32F)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_size, m_size, 0, GL_RED, GL_FLOAT, terrainMap.data());

    glBindTexture(GL_TEXTURE_2D, 0);
}
