#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

layout (std140) uniform viewUniform {
    mat4 view;
    mat4 projection;
};

uniform mat4 model;

out vec2 TextureCoord;
out vec3 Normal;

const int MAX_BONES = 100;

uniform mat4 gBones[MAX_BONES];

void main()
{
    mat4 BoneTransform;

    // Check if the vertex has any bone weights assigned
    if (aWeights[0] == 0.0 && aWeights[1] == 0.0 && aWeights[2] == 0.0 && aWeights[3] == 0.0) {
        BoneTransform = mat4(1.0); // Identity matrix (leaves position unchanged)
    } else {
        BoneTransform = gBones[aBoneIDs[0]] * aWeights[0];
        BoneTransform += gBones[aBoneIDs[1]] * aWeights[1];
        BoneTransform += gBones[aBoneIDs[2]] * aWeights[2];
        BoneTransform += gBones[aBoneIDs[3]] * aWeights[3];
    }

    vec4 bonedPosition = BoneTransform * vec4(aPosition, 1.0);

    vec3 FragPos = vec3(model * bonedPosition);
    gl_Position =  projection * view  * vec4(FragPos, 1.0);

    TextureCoord = aTextureCoord;

    // 2. Fix Normals: Transform the normal by the bones first, then the model matrix
    mat3 boneNormalMatrix = mat3(BoneTransform);
    vec3 animatedNormal = boneNormalMatrix * aNormal;

    // 3. Move to World Space (Using a CPU-calculated normalMatrix is ideal here if model has non-uniform scaling)
    Normal = normalize(mat3(model) * animatedNormal);
}