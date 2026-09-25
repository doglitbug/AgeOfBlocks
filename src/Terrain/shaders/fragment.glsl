#version 330

in vec2 TextureCoord;

out vec4 FragColor;

uniform sampler2D gSampler;

void main()
{
    FragColor = texture(gSampler, TextureCoord);
}