#pragma once

#include <vector>

#include "ObjectInstanced.h"
#include "TextureArray.h"
#include "glm/vec3.hpp"
#include "Resources/shaders/StaticShader.h"
#include "World/Terrain/shaders/TerrainShader.h"

// Make sure this is a power of 2, for SIMD optimization later on?
#define CHUNK_SIZE 32

// Scale grid size, in case 1 unit is too small
#define GRID_SIZE 1.0f

#define t00 glm::vec2(0.0f, 0.0f) // Bottom left
#define t01 glm::vec2(0.0f, 1.0f) // Top left
#define t10 glm::vec2(1.0f, 0.0f) // Bottom right
#define t11 glm::vec2(1.0f, 1.0f) // Top right

class map
{
public:
    enum TerrainType
    {
        MapEdge = 0,
        Grass,
        Rock,
        Sand,
        Water,
        DeepWater,
        SIZE,
    };

    enum StaticObjectType
    {
        TREE = 1
    };

    struct StaticObject
    {
        uint32_t entityID;
        glm::vec3 position;
        StaticObjectType type;
        int health;
    };

    struct Tile
    {
        TerrainType terrain;
        float height; // height of the bottom left corner
        uint32_t entityID;
    };



    //TODO Ensure rows/columns are a multiple of CHUNK_SIZE?
    explicit map(int size = CHUNK_SIZE);
    void render();
    [[nodiscard]] int getSize() const { return m_size; }

    /**
     * This will fill m_cells with all the data required for a new game, ideally the same as loading from a file
     * Of course, we will need to throw a heap of parameters: e.g. map type, player count as these will affect map generation
     */
    void generateMap();

    /**
     * Return the terrain height at the provided world co-ords
     * @param x
     * @param z
     * @return Averaged height (not 100% accurate as yet)
     */
    float getHeight(float x, float z) const;
    /**
     * Return if something can walk at the provided world co-ords
     * @param x
     * @param z
     * @return
     */
    bool isWalkable(float x, float z) const;

private:
    void loadTextures();
    void CreateTerrainMapTexture();
    void CreateHeightMapTexture();
    int m_size;
    std::vector<Tile> m_grid;
    std::vector<StaticObject> m_trees;
    ObjectInstanced m_treesInstance;

    TextureArray* m_textureArray;
    //TODO Should this just be a pointer?
    TerrainShader m_terrainShaderProgram;
    StaticShader m_TreeShader;

    GLuint m_VAO;
    GLuint heightTextureID;
    GLuint terrainTextureID;
};
