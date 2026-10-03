#version 330 core

layout (std140) uniform viewUniform {
    mat4 view;
    mat4 projection;
};

out vec2 TextureCoord;
out vec2 GridCoords;

uniform int mapSize;
uniform sampler2D heightMap;

void main() {
    // 1. Generate local quad vertex positions on-the-fly using gl_VertexID
    // Maps indices 0-5 to a CCW 2-triangle unit square from (0,0) to (1,1)
    vec2 localPos;
    switch (gl_VertexID) {
        case 0: localPos = vec2(0.0, 1.0); break;  // Top-Left
        case 1: localPos = vec2(1.0, 1.0); break;  // Top-Right
        case 2: localPos = vec2(1.0, 0.0); break;  // Bottom-Right

        case 3: localPos = vec2(0.0, 1.0); break;  // Top-Left
        case 4: localPos = vec2(1.0, 0.0); break;  // Bottom-Right
        case 5: localPos = vec2(0.0, 0.0); break;  // Bottom-Left
    }

    vec2 mapDimensions = vec2(mapSize, mapSize);

    // 2. Calculate this tile's column and row positions based on the instance index
    int gridX = gl_InstanceID % mapSize;
    int gridY = gl_InstanceID / mapSize;
    vec2 cellOffset = vec2(float(gridX), float(gridY));

    // 3. Combine them to get the absolute flat world coordinate
    vec2 worldPos2D = cellOffset + localPos;

    // 4. Look up vertical elevation using our continuous heightmap
    vec2 heightUV = worldPos2D / mapDimensions;
    float finalHeight = textureLod(heightMap, heightUV, 0.0).r;

    // 5. Final positioning output to clip space
    gl_Position = projection * view * vec4(worldPos2D.x, finalHeight, worldPos2D.y, 1.0);

    // 6. Forward UV maps to the fragment shader
    TextureCoord = localPos;
    GridCoords = worldPos2D / mapDimensions;
}