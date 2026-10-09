#version 330

in vec2 TextureCoord;
in vec2 GridCoords;
in vec2 FragPos;

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
uniform bool showGrid;
uniform vec3 gridColor = vec3(0.0, 0.0, 0.0); // Black grid lines (red if cannot place?)
uniform float gridSpacing = 1.0;              // Distance between lines
uniform float lineWidth = 1.0;                // Thickness of lines

out vec4 FragColor;

vec4 getLine(vec4 baseColor) {

    // 2. Calculate the grid cell location using X and Z axes
    vec2 gridCoords = FragPos / gridSpacing;

    // 3. Use fract() to find how close the fragment is to a grid line edge
    vec2 gridLines = fract(gridCoords - 0.5);
    vec2 derive = fwidth(gridCoords); // Screen-space derivative for anti-aliasing
    vec2 gridAA = abs(gridLines - 0.5) / derive;

    // 4. Find the minimum distance to a line axis
    float lineFactor = min(gridAA.x, gridAA.y);

    // Smooth out the line edges (Anti-Aliasing)
    float gridIntensity = 1.0 - min(lineFactor / lineWidth, 1.0);

    // 5. Blend the grid lines smoothly over your terrain texture
    vec3 finalColor = mix(baseColor.rgb, gridColor, gridIntensity * 0.7); // 0.7 controlling opacity

    return vec4(finalColor, baseColor.a);
}


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

    FragColor = showGrid ? getLine(texColor) : texColor;
}

