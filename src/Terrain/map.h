#pragma once

#include <vector>

#include "TextureArray.h"
#include "shaders/TerrainShader.h"

// Make sure this is a power of 2, for SIMD optimization later on?
#define CHUNK_SIZE 32

// Scale grid size, in case 1 unit is too small
#define GRID_SIZE 2.0f

#define t00 glm::vec2(0.0f, 0.0f) // Bottom left
#define t01 glm::vec2(0.0f, 1.0f) // Top left
#define t10 glm::vec2(1.0f, 0.0f) // Bottom right
#define t11 glm::vec2(1.0f, 1.0f) // Top right

enum terrainType
{
    MapEdge = 0,
    Grass,
    Rock,
    Sand,
};

struct cell
{
    terrainType terrain;
    bool walkable;
    bool isBuildable;
};

static constexpr cell OutOfBoundsCell{MapEdge, false, false};

class map
{
public:
    //TODO Ensure rows/columns are a multiple of CHUNK_SIZE?
    explicit map(int size = CHUNK_SIZE);

    void render();

    void generateMap();

    [[nodiscard]] int getSize() const { return m_size; }

private:
    void loadTextures();
    void CreateHeightMapTexture();
    void CreateTerrainMapTexture();
    int m_size;
    std::vector<float> heightMap;
    std::vector<float> terrainMap;

    TextureArray *m_textureArray;
    //TODO Should this just be a pointer?
    TerrainShader m_terrainShaderProgram;
    GLuint m_VAO;
    GLuint heightTextureID;
    GLuint terrainTextureID;
};