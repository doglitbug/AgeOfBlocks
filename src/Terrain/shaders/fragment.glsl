#version 330

in vec2 TexCoord0;

out vec4 FragColor;

uniform sampler2D gSampler;

void main()
{
    //FragColor = texture(gSampler, TexCoord0);
    FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}