#version 330

in vec2 TextureCoord;
in vec3 Normal;
flat in int TerrainIndex;

layout (std140) uniform lightingUniform {
    vec3 ambientColor;
    float _pad0; // or intensity if we have that as a setting?
    vec3 lightDirection;
    float _pad1;
    vec3 lightColor;
    float _pad2;
};

out vec4 FragColor;

uniform sampler2DArray textureArray;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 negLightDirection = normalize(-lightDirection);

    vec4 texColor = texture(textureArray, vec3(TextureCoord, float(TerrainIndex)));

    vec3 ambient = ambientColor * texColor.rgb;

    float diff = max(dot(norm, negLightDirection), 0.0);
    vec3 diffuse = diff * lightColor * texColor.rgb;

    vec3 result = ambient + diffuse;

    // Preserve the original texture alpha channel
    FragColor = vec4(result, texColor.a);
}