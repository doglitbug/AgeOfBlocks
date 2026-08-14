#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TextureCoord;

uniform mat4 gTranslate;
uniform mat4 gCamera;

out vec2 TextureCoord0;

void main()
{
    gl_Position =  gCamera * gTranslate * vec4(Position, 1.0);
    TextureCoord0 = TextureCoord;
}