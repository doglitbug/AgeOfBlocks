#include "map.h"

#include <cmath>
#include <random>

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
    glUniform1f(m_terrainShaderProgram.getUniformLocation("heightScale"), 2.0f);

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
    constexpr siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{seed};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(1, terrainType::SIZE);

    m_cells.resize(m_size * m_size);

    for (int y = 0; y < m_size; ++y)
    {
        for (int x = 0; x < m_size; ++x)
        {
            auto height = static_cast<float>(perlin.octave2D_01((x * 0.05), (y * 0.05), 2) * 4.0f - 1.75f);
            if (height < 0) height = 0.0f; // Large flat areas, but will need to add in water in another run?

            int random_num = distrib(gen);
            m_cells[y * m_size + x] = cell(static_cast<terrainType>(random_num), height, true, true);
        }
    }

    CreateTerrainMapTexture();
    CreateHeightMapTexture();
}

void map::CreateTerrainMapTexture()
{
    std::vector<float> terrainMap(m_size * m_size);
    for (size_t z = 0; z < m_size * m_size; ++z)
    {
        terrainMap[z] = m_cells[z].terrain;
    }

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

void map::CreateHeightMapTexture()
{
    const int vWidth = m_size + 1;
    const int vHeight = m_size + 1;

    std::vector<float> heightMap(vWidth * vHeight);

    // 1. Fill the main inner grid
    for (int y = 0; y < m_size; ++y)
    {
        for (int x = 0; x < m_size; ++x)
        {
            heightMap[y * vWidth + x] = m_cells[y * m_size + x].height;
        }
    }

    // 2. Handle the edge/fence-post copying
    for (int i = 0; i < (m_size + 1); ++i)
    {
        // Bottom edge (y = m_size): Copy from the row directly above it (y = m_size - 1)
        heightMap[m_size * vWidth + i] = heightMap[(m_size - 1) * vWidth + i];

        // Right edge (x = m_size): Copy from the column directly to its left (x = m_size - 1)
        heightMap[i * vWidth + m_size] = heightMap[i * vWidth + (m_size - 1)];
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


