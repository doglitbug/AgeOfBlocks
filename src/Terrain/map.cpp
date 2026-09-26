#include "map.h"

#include <SDL3/SDL_log.h>

void map::populateBuffers()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    generateWallVertices();
    generateGroundVertices();

    std::vector<vertex> totalVertices;
    totalVertices.reserve(wallVertices.size() + groundVertices.size());

    // Append wall data
    totalVertices.insert(totalVertices.end(), wallVertices.begin(), wallVertices.end());
    // Append ground data
    totalVertices.insert(totalVertices.end(), groundVertices.begin(), groundVertices.end());


    glGenBuffers(1, &m_VBO);
    // Walls
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, totalVertices.size() * sizeof(vertex), totalVertices.data(), GL_STATIC_DRAW);


    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), nullptr);

    // tex coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          reinterpret_cast<const void*>(offsetof(vertex, textureCoordinate)));

    // normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(vertex),
                          reinterpret_cast<const void*>(offsetof(vertex, normal)));

    // terrain type
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(vertex),
                           reinterpret_cast<const void*>(offsetof(vertex, terrainType)));



    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void map::generateWallVertices()
{
    wallVertices.reserve(size * 4 * 6);

    // Top and bottom of walls
    constexpr float y_bottom = 0.0f;
    constexpr float y_top = y_bottom + GRID_SIZE;

    constexpr int terrainLayer = MapEdge;

    // North wall
    for (int x = 0; x < size; ++x)
    {
        const float z = size * GRID_SIZE;
        auto normal = glm::vec3(0.0f, 0.0f, -1.0f);
        // Pre-calculate physical geometric bounds for this specific quad step
        float x_left = x * GRID_SIZE;
        float x_right = (x + 1) * GRID_SIZE;

        // Triangle 1: LHS (Bottom-Left -> Top-Left -> Top-Right)
        wallVertices.emplace_back(glm::vec3(x_left, y_bottom, z), t00, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x_left, y_top, z), t01, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x_right, y_top, z), t11, normal, terrainLayer);
        // Standard Top-Right mapping

        // Triangle 2: RHS (Bottom-Left -> Top-Right -> Bottom-Right)
        wallVertices.emplace_back(glm::vec3(x_left, y_bottom, z), t00, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x_right, y_top, z), t11, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x_right, y_bottom, z), t10, normal, terrainLayer);
        // Corrected Bottom-Right mapping
    }

    // East wall
    for (int z = size; z > 0; --z)
    {
        const float x = size * GRID_SIZE;
        auto normal = glm::vec3(-1.0f, 0.0f, 0.0f);
        // Pre-calculate physical geometric bounds for this specific quad step
        float z_left = z * GRID_SIZE;
        float z_right = (z - 1) * GRID_SIZE;

        // Triangle 1: LHS (Bottom-Left -> Top-Left -> Top-Right)
        wallVertices.emplace_back(glm::vec3(x, y_bottom, z_left), t00, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x, y_top, z_left), t01, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x, y_top, z_right), t11, normal, terrainLayer);
        // Standard Top-Right mapping

        // Triangle 2: RHS (Bottom-Left -> Top-Right -> Bottom-Right)
        wallVertices.emplace_back(glm::vec3(x, y_bottom, z_left), t00, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x, y_top, z_right), t11, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x, y_bottom, z_right), t10, normal, terrainLayer);
        // Corrected Bottom-Right mapping
    }

    // South wall
    for (int x = size; x > 0; --x)
    {
        const float z = 0.0f;
        auto normal = glm::vec3(0.0f, 0.0f, 1.0f);
        // Pre-calculate physical geometric bounds for this specific quad step
        float x_left = x * GRID_SIZE;
        float x_right = (x - 1) * GRID_SIZE;

        // Triangle 1: LHS (Bottom-Left -> Top-Left -> Top-Right)
        wallVertices.emplace_back(glm::vec3(x_left, y_bottom, z), t00, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x_left, y_top, z), t01, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x_right, y_top, z), t11, normal, terrainLayer);
        // Standard Top-Right mapping

        // Triangle 2: RHS (Bottom-Left -> Top-Right -> Bottom-Right)
        wallVertices.emplace_back(glm::vec3(x_left, y_bottom, z), t00, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x_right, y_top, z), t11, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x_right, y_bottom, z), t10, normal, terrainLayer);
        // Corrected Bottom-Right mapping
    }

    //West wall
    for (int z = 0; z < size; ++z)
    {
        const float x = 0.0f;
        auto normal = glm::vec3(1.0f, 0.0f, 0.0f);
        // Pre-calculate physical geometric bounds for this specific quad step
        float z_left = z * GRID_SIZE;
        float z_right = (z + 1) * GRID_SIZE;

        // Triangle 1: LHS (Bottom-Left -> Top-Left -> Top-Right)
        wallVertices.emplace_back(glm::vec3(x, y_bottom, z_left), t00, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x, y_top, z_left), t01, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x, y_top, z_right), t11, normal, terrainLayer);
        // Standard Top-Right mapping

        // Triangle 2: RHS (Bottom-Left -> Top-Right -> Bottom-Right)
        wallVertices.emplace_back(glm::vec3(x, y_bottom, z_left), t00, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x, y_top, z_right), t11, normal, terrainLayer);
        wallVertices.emplace_back(glm::vec3(x, y_bottom, z_right), t10, normal, terrainLayer);
        // Corrected Bottom-Right mapping
    }
}

void map::generateGroundVertices()
{
    groundVertices.reserve(size * size * 6);

    auto normal = glm::vec3(0.0f, 1.0f, 0.0f);
    for (int z = 0; z < size; ++z)
    {
        // Pre-calculate physical geometric bounds for this specific quad step
        float z_top = (z + 1) * GRID_SIZE;
        float z_bottom = z * GRID_SIZE;
        for (int x = 0; x < size; ++x)
        {
            // Pre-calculate physical geometric bounds for this specific quad step
            float x_left = x * GRID_SIZE;
            float x_right = (x + 1) * GRID_SIZE;

            int terrainLayer = (*this)(z, x).terrain;

            // Triangle 1: LHS (Bottom-Left -> Top-Left -> Top-Right)
            groundVertices.emplace_back(glm::vec3(x_left, 0, z_bottom), t00, normal, terrainLayer);
            groundVertices.emplace_back(glm::vec3(x_left, 0, z_top), t01, normal, terrainLayer);
            groundVertices.emplace_back(glm::vec3(x_right, 0, z_top), t11, normal, terrainLayer);
            // Standard Top-Right mapping

            // Triangle 2: RHS (Bottom-Left -> Top-Right -> Bottom-Right)
            groundVertices.emplace_back(glm::vec3(x_left, 0, z_bottom), t00, normal, terrainLayer);
            groundVertices.emplace_back(glm::vec3(x_right, 0, z_top), t11, normal, terrainLayer);
            groundVertices.emplace_back(glm::vec3(x_right, 0, z_bottom), t10, normal, terrainLayer);
            // Corrected Bottom-Right mapping
        }
    }
}

void map::loadTextures()
{
    const auto terrainPath = "assets/terrain/";
    const std::vector<std::string> filenames = {
        "RockWall_Texture_01.png",
        "Grass_Clovers_Texture_01.png",
         "Sand_Texture_01.png"
    };

    m_textureArray = new TextureArray(terrainPath, filenames);
    m_textureArray->Load();
}

void map::render()
{
    glBindVertexArray(m_VAO);

    m_textureArray->Bind(GL_TEXTURE0);

    glDrawArrays(GL_TRIANGLES, 0, wallVertices.size() + groundVertices.size());

    glBindVertexArray(0);
}

void map::generateMap()
{
    grid.resize(size * size);

    for (int z = 0; z < size; ++z)
    {
        for (int x = 0; x < size; ++x)
        {
            auto terrainType = Grass;
            if (x<2 || z < 2) { terrainType = Sand; }
            (*this)(z,x) = cell(terrainType, 0, true, true);
        }
    }
}
