#version 330

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aNormal;

layout (std140) uniform viewUniform {
    mat4 view;
    mat4 projection;
};

out vec2 TextureCoord;
out vec3 Normal;

void main()
{
    gl_Position = projection * view * vec4(aPosition, 1.0);

    TextureCoord = aTextureCoord;
    Normal = aNormal;
}