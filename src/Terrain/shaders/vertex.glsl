#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 aTextureCoord;

layout (std140) uniform shared {
    mat4 view;
    mat4 projection;
};

out vec2 TextureCoord;

void main()
{
    gl_Position = projection * view * vec4(Position, 1.0);
    TextureCoord = aTextureCoord;
}