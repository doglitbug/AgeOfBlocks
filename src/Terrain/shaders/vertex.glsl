#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

layout (std140) uniform shared {
    mat4 view;
    mat4 projection;
};

out vec2 TexCoord0;

void main()
{
    gl_Position = projection * view * vec4(Position, 1.0);
    TexCoord0 = TexCoord;
}