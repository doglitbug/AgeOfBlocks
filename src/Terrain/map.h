#pragma once

#include <vector>

#include "ObjectBase.h"
#include "TextureArray.h"
#include "glm/vec2.hpp"

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
    Sand
};

struct cell
{
    terrainType terrain;
    int height;
    bool walkable;
    bool isBuildable;
};

struct vertex
{
    glm::vec3 position{};
    glm::vec2 textureCoordinate{};
    glm::vec3 normal{};
    int terrainType{};

    vertex(const glm::vec3 position, const glm::vec2 textureCoordinate, const glm::vec3 normal, const int terrainType)
    {
        this->position = position;
        this->textureCoordinate = textureCoordinate;
        this->normal = normal;
        this->terrainType = terrainType;
    }
};

static constexpr cell OutOfBoundsCell{MapEdge, 10, false, false};

class map : public ObjectBase
{
public:
    //TODO Ensure rows/columns are a multiple of CHUNK_SIZE?
    explicit map(const int size = CHUNK_SIZE) : size(size)
    {
        loadTextures();
    }

    void populateBuffers() override;
    void render() override;

    void generateMap();

    [[nodiscard]] int getSize() const { return size; }

    // Mutable reference
    cell& operator()(const int row, const int col)
    {
        return grid[row * size + col];
    }

    // Read only
    const cell& operator()(const int row, const int col) const
    {
        return grid[row * size + col];
    }

private:
    void generateWallVertices();
    void generateGroundVertices();
    //TODO Move to parent?
    void loadTextures();
    int size;
    std::vector<cell> grid;
    TextureArray *m_textureArray;
    // Global because they will likely change due to height during game play?
    std::vector<vertex> wallVertices;
    std::vector<vertex> groundVertices;
};


