#pragma once

#include <vector>

#include "ObjectBase.h"
#include "Texture.h"
#include "glm/vec2.hpp"

// Make sure this is a power of 2, for SIMD optimization later on?
#define CHUNK_SIZE 32

// Scale grid size, in case 1 unit is too small
#define GRID_SIZE 2.0f

struct vertex
{
    glm::vec3 position;
    glm::vec2 textureCoordinate;
    glm::vec3 normal;

    vertex(const glm::vec3 position, const glm::vec2 textureCoordinate, const glm::vec3 normal)
    {
        this->position = position;
        this->textureCoordinate = textureCoordinate;
        this->normal = normal;
    }
};
enum terrainType
{
    OutOfBounds,
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

static constexpr cell OutOfBoundsCell{OutOfBounds, 10, false, false};

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
        // if (row < 0 || row > rows || col < 0 || col > cols)
        // {
        //     return OutOfBoundsCell; // TODO Do we need this, or replace with assert?
        // }
        return grid[row * size + col];
    }

private:
    //TODO Move to parent?
    void loadTextures();
    int size;
    std::vector<cell> grid;
    std::vector<Texture *> m_textures;
};


