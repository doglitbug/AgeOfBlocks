#version 330 core

in vec2 TextureCoord0;

out vec4 FragColor;

uniform sampler2D gSampler;
uniform vec3 ambientColor;
uniform float ambientIntensity;

void main()
{
    // 1. Calculate the ambient light factor
    vec3 ambient = ambientIntensity * ambientColor;

    // 2. Sample the base color from the texture map
    vec4 texColor = texture(gSampler, TextureCoord0);

    // 3. Multiply the texture color by the ambient light
    vec3 result = ambient * texColor.rgb;

    // 4. Output the final color, preserving the original texture alpha channel
    FragColor = vec4(result, texColor.a);
}