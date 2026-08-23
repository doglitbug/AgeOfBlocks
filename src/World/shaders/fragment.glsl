#version 330 core

in vec2 TextureCoord0;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D gSampler;
uniform vec3 ambientColor;
uniform float ambientIntensity;

uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform vec3 gNormal;

void main()
{
    vec3 norm = normalize(Normal);

    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 ambient = ambientIntensity * ambientColor;

    vec4 texColor = texture(gSampler, TextureCoord0);

    vec3 result = (ambient + diffuse) * texColor.rgb;

    // 4. Output the final color, preserving the original texture alpha channel
    FragColor = vec4(result, texColor.a);
}