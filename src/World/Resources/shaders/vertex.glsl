#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aInstancedPosition;

layout (std140) uniform viewUniform {
    mat4 view;
    mat4 projection;
};

out vec2 TextureCoord;
out vec3 Normal;

void main()
{
    // Combine local vertex position with the instance's world position offset
    vec3 worldPosition = aPosition + aInstancedPosition;

    // Transform directly from World Space to Clip Space
    gl_Position = projection * view * vec4(worldPosition, 1.0);

    TextureCoord = aTextureCoord;

    // Pass the normal through directly (no model matrix means no rotation needed)
    Normal = normalize(aNormal);
}