#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

uniform mat4 model;
uniform mat4 gCamera;

out vec3 FragPos;
out vec2 TextureCoord;
out vec3 Normal;
flat out ivec4 BoneIDs;
out vec4 Weights;

const int MAX_BONES = 100;

uniform mat4 gBones[MAX_BONES];
void main()
{
    mat4 BoneTransform = gBones[aBoneIDs[0]] * aWeights[0];
    BoneTransform     += gBones[aBoneIDs[1]] * aWeights[1];
    BoneTransform     += gBones[aBoneIDs[2]] * aWeights[2];
    BoneTransform     += gBones[aBoneIDs[3]] * aWeights[3];

    vec4 bonedPosition = BoneTransform * vec4(aPosition, 1.0);

    FragPos = vec3(model * bonedPosition);
    gl_Position =  gCamera * vec4(FragPos, 1.0);

    TextureCoord = aTextureCoord;
    Normal = normalize(mat3(model) * aNormal);

    BoneIDs = aBoneIDs;
    Weights = aWeights;
}