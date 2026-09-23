#include "map.h"

void map::populateBuffers()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    //Build walls
    std::vector<vertex> vertices;
    vertices.reserve(size * 4 * 6);

    constexpr auto t00 = glm::vec2(0.0f, 0.0f); // Bottom left
    constexpr auto t01 = glm::vec2(0.0f, 1.0f); // Top left
    constexpr auto t10 = glm::vec2(1.0f, 0.0f); // Bottom right
    constexpr auto t11 = glm::vec2(1.0f, 1.0f); // Top right

    // Top and bottom of walls
    constexpr float y_bottom = 0.0f;
    constexpr float y_top    = y_bottom + GRID_SIZE;

    // North wall
    for (int x = 0; x < size; ++x)
    {
        const float z = size * GRID_SIZE;
        // Pre-calculate physical geometric bounds for this specific quad step
        float x_left  = x * GRID_SIZE;
        float x_right = (x + 1) * GRID_SIZE;

        // Triangle 1: LHS (Bottom-Left -> Top-Left -> Top-Right)
        vertices.push_back(vertex(glm::vec3(x_left,  y_bottom, z), t00));
        vertices.push_back(vertex(glm::vec3(x_left,  y_top,    z), t01));
        vertices.push_back(vertex(glm::vec3(x_right, y_top,    z), t11)); // Standard Top-Right mapping

        // Triangle 2: RHS (Bottom-Left -> Top-Right -> Bottom-Right)
        vertices.push_back(vertex(glm::vec3(x_left,  y_bottom, z), t00));
        vertices.push_back(vertex(glm::vec3(x_right, y_top,    z), t11));
        vertices.push_back(vertex(glm::vec3(x_right, y_bottom, z), t10)); // Corrected Bottom-Right mapping
    }

    // East wall
    for (int z = size; z > 0; --z)
    {
        const float x = size * GRID_SIZE;
        // Pre-calculate physical geometric bounds for this specific quad step
        float z_left  = z * GRID_SIZE;
        float z_right = (z - 1) * GRID_SIZE;

        // Triangle 1: LHS (Bottom-Left -> Top-Left -> Top-Right)
        vertices.push_back(vertex(glm::vec3(x,  y_bottom, z_left), t00));
        vertices.push_back(vertex(glm::vec3(x,  y_top,    z_left), t01));
        vertices.push_back(vertex(glm::vec3(x, y_top,    z_right), t11)); // Standard Top-Right mapping

        // Triangle 2: RHS (Bottom-Left -> Top-Right -> Bottom-Right)
        vertices.push_back(vertex(glm::vec3(x,  y_bottom, z_left), t00));
        vertices.push_back(vertex(glm::vec3(x, y_top,    z_right), t11));
        vertices.push_back(vertex(glm::vec3(x, y_bottom, z_right), t10)); // Corrected Bottom-Right mapping
    }

    // South wall
    for (int x = size; x > 0; --x)
    {
        const float z = 0.0f;
        // Pre-calculate physical geometric bounds for this specific quad step
        float x_left  = x * GRID_SIZE;
        float x_right = (x -1) * GRID_SIZE;

        // Triangle 1: LHS (Bottom-Left -> Top-Left -> Top-Right)
        vertices.push_back(vertex(glm::vec3(x_left,  y_bottom, z), t00));
        vertices.push_back(vertex(glm::vec3(x_left,  y_top,    z), t01));
        vertices.push_back(vertex(glm::vec3(x_right, y_top,    z), t11)); // Standard Top-Right mapping

        // Triangle 2: RHS (Bottom-Left -> Top-Right -> Bottom-Right)
        vertices.push_back(vertex(glm::vec3(x_left,  y_bottom, z), t00));
        vertices.push_back(vertex(glm::vec3(x_right, y_top,    z), t11));
        vertices.push_back(vertex(glm::vec3(x_right, y_bottom, z), t10)); // Corrected Bottom-Right mapping
    }

    //West wall
    for (int z = 0; z < size; ++z)
    {
        const float x = 0.0f;
        // Pre-calculate physical geometric bounds for this specific quad step
        float z_left  = z * GRID_SIZE;
        float z_right = (z + 1) * GRID_SIZE;

        // Triangle 1: LHS (Bottom-Left -> Top-Left -> Top-Right)
        vertices.push_back(vertex(glm::vec3(x,  y_bottom, z_left), t00));
        vertices.push_back(vertex(glm::vec3(x,  y_top,    z_left), t01));
        vertices.push_back(vertex(glm::vec3(x, y_top,    z_right), t11)); // Standard Top-Right mapping

        // Triangle 2: RHS (Bottom-Left -> Top-Right -> Bottom-Right)
        vertices.push_back(vertex(glm::vec3(x,  y_bottom, z_left), t00));
        vertices.push_back(vertex(glm::vec3(x, y_top,    z_right), t11));
        vertices.push_back(vertex(glm::vec3(x, y_bottom, z_right), t10)); // Corrected Bottom-Right mapping
    }

    //Build ground TODO

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);

    // tex coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(sizeof(glm::vec3)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void map::render()
{
    glBindVertexArray(m_VAO);

    //TODO Bind textures

    //TODO Dynamic count here
    glDrawArrays(GL_LINES,0, size*6*4);

    glBindVertexArray(0);
}

void map::generateMap()
{
    grid.resize(size * size);

    for (auto& i : grid)
    {
        i = cell(Grass, 0, true, true);
    }
}
