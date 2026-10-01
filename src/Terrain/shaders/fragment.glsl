#version 330

in vec2 TextureCoord;
in vec2 GridCoords;

layout (std140) uniform lightingUniform {
    vec3 ambientColor;
    float _pad0; // or intensity if we have that as a setting?
    vec3 lightDirection;
    float _pad1;
    vec3 lightColor;
    float _pad2;
};

uniform sampler2DArray textureArray;
uniform sampler2D terrainMap;

out vec4 FragColor;

void main()
{
    // 1. Read the raw 32-bit float value directly from your map.
    // Because your format is GL_R32F, this is already the exact layer index (e.g. 0.0, 1.0, 2.0)
    float terrainID = texture(terrainMap, GridCoords).r;

    // 2. Pass it directly to the texture array.
    // We add a tiny rounding protection (floor + 0.5) just in case floating-point precision
    // makes a 1.0 read as 0.99999 from the texture cache.
    float safeLayerIndex = floor(terrainID + 0.5);

    vec4 texColor = texture(textureArray, vec3(TextureCoord, safeLayerIndex));

    FragColor = texColor;
}