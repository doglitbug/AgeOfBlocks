#version 330 core

in vec2 TextureCoord0;

out vec4 FragColor;

uniform sampler2D gSampler;

void main()
{
    FragColor = texture2D(gSampler, TextureCoord0);
}