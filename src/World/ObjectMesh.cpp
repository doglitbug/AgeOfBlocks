#include "ObjectMesh.h"

#include <assimp/postprocess.h>

#include <iostream>
#include <filesystem>
#include <SDL3/SDL_log.h>

#include "glm/detail/type_quat.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#define DEBUG(x) std::cout << "Debug: " << x << std::endl;

ObjectMesh::ObjectMesh()
{
    m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    m_scale = 1.0f;
    m_VAO = -1;
}

bool ObjectMesh::LoadMesh(const std::string &filename)
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);

    //TODO Remove from being global
    //Assimp::Importer importer

    //TODO factor pScene out?
    pScene = importer.ReadFile(filename.c_str(), ASSIMP_LOAD_FLAGS);

    if (!pScene)
    {
        std::cerr << "Unable to parse model: " << filename << ": " << importer.GetErrorString() << std::endl;
        return false;
    }

    LoadFromFile(pScene, filename);

    PopulateBuffers();

    glBindVertexArray(0);
    return true;
}

void ObjectMesh::Render(const unsigned int meshIndex) const
{
    // TODO If meshIndex = -1, render all?
    glBindVertexArray(m_VAO);

    const unsigned int i = meshIndex;
    const unsigned int materialIndex = m_meshes[i].materialIndex;
    if (materialIndex < m_textures.size())
    {
        m_textures[materialIndex]->Bind(COLOR_TEXTURE_UNIT);
    }
    glDrawElementsBaseVertex(GL_TRIANGLES,
                             m_meshes[i].numberIndices,
                             GL_UNSIGNED_INT,
                             (void *)(sizeof(unsigned int) * m_meshes[i].startingIndex),
                             m_meshes[i].startingVertex);

    glBindVertexArray(0);
}

glm::mat4 ObjectMesh::GetWorldMatrix() const
{
    constexpr auto identity = glm::mat4(1.0f);
    const auto translation = glm::translate(identity, m_position);
    const auto rotation = glm::mat4_cast(glm::quat(glm::radians(m_rotation)));

    // Remember T * R * S
    return translation * rotation * m_scale;
}

void ObjectMesh::LoadFromFile(const aiScene *pScene, const std::string &filename)
{
    m_meshes.resize(pScene->mNumMeshes);
    m_textures.resize(pScene->mNumMaterials);

    unsigned int numberVertices = 0;
    unsigned int numberIndices = 0;

    CountVerticesAndIndices(pScene, numberVertices, numberIndices);
    ReserveSpace(numberVertices, numberIndices);

    // Load all meshes
    for (unsigned int i = 0; i < m_meshes.size(); i++)
    {
        const aiMesh *paiMesh = pScene->mMeshes[i];
        LoadMesh(i, paiMesh);
    }

    LoadMaterials(pScene, filename);
}

void ObjectMesh::CountVerticesAndIndices(const aiScene *pScene, unsigned int &numberVertices, unsigned int &numberIndices)
{
    for (unsigned int i = 0; i < m_meshes.size(); i++)
    {
        m_meshes[i].materialIndex = pScene->mMeshes[i]->mMaterialIndex;
        m_meshes[i].numberIndices = pScene->mMeshes[i]->mNumFaces * 3;
        m_meshes[i].startingVertex = numberVertices;
        m_meshes[i].startingIndex = numberIndices;

        numberVertices += pScene->mMeshes[i]->mNumVertices;
        numberIndices += m_meshes[i].numberIndices;
    }
}

void ObjectMesh::ReserveSpace(const unsigned int numberVertices, const unsigned int numberIndices)
{
    m_indices.reserve(numberIndices);
    m_positions.reserve(numberVertices);
    m_textureCoords.reserve(numberVertices);
    m_normals.reserve(numberVertices);
    m_bones.resize(numberVertices);
}

void ObjectMesh::LoadMesh(const uint meshIndex, const aiMesh *paiMesh)
{
    // Populate vertex attribute vectors
    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
    {
        const aiVector3D &pPos = paiMesh->mVertices[i];
        const aiVector3D &pTextureCoords = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);
        const aiVector3D &pNormal = paiMesh->mNormals ? paiMesh->mNormals[i]:aiVector3D(0.0f, 1.0f, 0.0f);

        // A -90-degree rotation around X changes: (X, Y, Z) -> (X, Z, -Y)
        //m_positions.emplace_back(pPos.x, pPos.z, -pPos.y);
        m_positions.emplace_back(pPos.x, pPos.y, pPos.z);
        m_textureCoords.emplace_back(pTextureCoords.x, pTextureCoords.y);
        m_normals.emplace_back(pNormal.x, pNormal.y, pNormal.z);
    }

    // Populate the index buffer
    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
    {
        const aiFace &face = paiMesh->mFaces[i];
        m_indices.push_back(face.mIndices[0]);
        m_indices.push_back(face.mIndices[1]);
        m_indices.push_back(face.mIndices[2]);
    }

    // Populate the bones buffer
    if (paiMesh->HasBones())
    {
        for (int i = 0; i < paiMesh->mNumBones; i++)
        {
            LoadBone(meshIndex, paiMesh->mBones[i]);
        }
    }
}

void ObjectMesh::LoadBone(const uint meshIndex, const aiBone* pBone)
{
    const int boneId = GetBoneId(pBone);

    if (boneId == m_BoneInfo.size())
    {
        const BoneInfo bi(pBone->mOffsetMatrix);
        m_BoneInfo.push_back(bi);
    }

    for (uint i = 0 ; i < pBone->mNumWeights ; i++) {
        const aiVertexWeight& vw = pBone->mWeights[i];
        const uint GlobalVertexID = m_meshes[meshIndex].startingVertex + pBone->mWeights[i].mVertexId;
        m_bones[GlobalVertexID].AddBoneData(boneId, vw.mWeight);
    }
}

int ObjectMesh::GetBoneId(const aiBone* pBone)
{
    int BoneIndex = 0;
    std::string BoneName(pBone->mName.C_Str());

    if (m_BoneNameToIndexMap.find(BoneName) == m_BoneNameToIndexMap.end()) {
        // Allocate an index for a new bone
        BoneIndex = static_cast<int>(m_BoneNameToIndexMap.size());
        m_BoneNameToIndexMap[BoneName] = BoneIndex;
    }
    else {
        BoneIndex = m_BoneNameToIndexMap[BoneName];
    }

    return BoneIndex;
}

void ObjectMesh::GetBoneTransforms(std::vector<glm::mat4>& boneTransforms, const float animationTime)
{
    //TODO Remove hardcoded mAnimations number here
    auto ticksPerSecond = static_cast<float>(pScene->mAnimations[0]->mTicksPerSecond);
    if (ticksPerSecond == 0.0f) ticksPerSecond = 25.0f;

    const float timeInTicks = animationTime * ticksPerSecond;
    //TODO Remove this mod once we wrap the animationTime!
    const float animationTimeTicks = fmod(timeInTicks, static_cast<float>(pScene->mAnimations[0]->mDuration));

    constexpr auto identity = glm::mat4(1.f);
    ReadNodeHierarchy(animationTimeTicks, pScene->mRootNode, identity);
    boneTransforms.resize(m_BoneInfo.size());

    for (unsigned int i = 0; i < m_BoneInfo.size(); i++)
    {
        boneTransforms[i] = m_BoneInfo[i].finalTransformation;
    }
}

void ObjectMesh::ReadNodeHierarchy(const float animationTimeTicks, const aiNode* pNode, const glm::mat4& parentTransform)
{
    const std::string nodeName(pNode->mName.C_Str());
    const aiAnimation* pAnimation = pScene->mAnimations[0];
    glm::mat4 nodeTransform(aiMatrix4x4ToGlm(pNode->mTransformation));

    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);
    if (pNodeAnim)
    {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D scaling;
        CalculateInterpolatedScaling(scaling, animationTimeTicks, pNodeAnim);
        const glm::mat4 scalingM = glm::scale(glm::mat4(1.0f),glm::vec3(scaling.x, scaling.y, scaling.z));

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion rotation;
        CalculateInterpolatedRotation(rotation, animationTimeTicks, pNodeAnim);
        const glm::quat glmRotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
        const glm::mat4 rotationM = glm::mat4_cast(glmRotation);

        // Interpolate translation and generate translation transformation matrix
        aiVector3D translation;
        CalculateInterpolatedPosition(translation, animationTimeTicks, pNodeAnim);
        const glm::mat4 translationM = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z));

        // Combine
        nodeTransform = translationM * rotationM * scalingM;
    }

    const glm::mat4 globalTransform = parentTransform * nodeTransform;

    if (m_BoneNameToIndexMap.find(nodeName) != m_BoneNameToIndexMap.end())
    {
        const unsigned int boneIndex = m_BoneNameToIndexMap[nodeName];
        m_BoneInfo[boneIndex].finalTransformation = globalTransform * m_BoneInfo[boneIndex].offsetMatrix;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++)
    {
        ReadNodeHierarchy(animationTimeTicks, pNode->mChildren[i], globalTransform);
    }
}

void ObjectMesh::CalculateInterpolatedScaling(aiVector3D& out, float animationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1)
    {
        out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    unsigned int scalingIndex = FindScaling(animationTimeTicks, pNodeAnim);

    float t1 = pNodeAnim->mScalingKeys[scalingIndex].mTime;
    float t2 = pNodeAnim->mScalingKeys[scalingIndex+1].mTime;

    float factor = (animationTimeTicks - t1)/(t2 - t1);
    const aiVector3D& start = pNodeAnim->mScalingKeys[scalingIndex].mValue;
    const aiVector3D& end = pNodeAnim->mScalingKeys[scalingIndex+1].mValue;
    out = start + factor * (end-start);
}

unsigned int ObjectMesh::FindScaling(const float animationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
        if (animationTimeTicks < t) {
            return i;
        }
    }

    return 0;
}

void ObjectMesh::CalculateInterpolatedRotation(aiQuaternion& out, float animationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1) {
        out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint rotationIndex = FindRotation(animationTimeTicks, pNodeAnim);

    float t1 = (float)pNodeAnim->mRotationKeys[rotationIndex].mTime;
    float t2 = (float)pNodeAnim->mRotationKeys[rotationIndex+1].mTime;
    float factor = (animationTimeTicks - t1) / (t2-t1);

    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[rotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[rotationIndex+1].mValue;
    aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, factor);
    out.Normalize();
}

unsigned int ObjectMesh::FindRotation(float animationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
        if (animationTimeTicks < t) {
            return i;
        }
    }

    return 0;
}

void ObjectMesh::CalculateInterpolatedPosition(aiVector3D& out, float animationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumPositionKeys == 1) {
        out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint PositionIndex = FindPosition(animationTimeTicks, pNodeAnim);
    uint NextPositionIndex = PositionIndex + 1;

    float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
    float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor = (animationTimeTicks - t1) / DeltaTime;

    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    out = Start + Factor * (End - Start);
}

unsigned int ObjectMesh::FindPosition(float animationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
        if (animationTimeTicks < t) {
            return i;
        }
    }

    return 0;
}

const aiNodeAnim* ObjectMesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName)
{
    for (uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == nodeName) {
            return pNodeAnim;
        }
    }

    return nullptr;
}

void ObjectMesh::LoadMaterials(const aiScene *xpScene, const std::string &filename)
{
    // Get directory path from filename
    std::filesystem::path filePath(filename);
    filePath = filePath.parent_path();

    for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
    {
        const aiMaterial *pMaterial = pScene->mMaterials[i];

        m_textures[i] = nullptr;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
            {
                auto fullPath = filePath / path.data;
                m_textures[i] = new Texture(GL_TEXTURE_2D, fullPath);
                if (!m_textures[i]->Load())
                {
                    std::cerr << "Unable to load texture at " << fullPath << std::endl;
                    delete m_textures[i];
                    m_textures[i] = nullptr;
                }
            }
        }
    }
}

void ObjectMesh::PopulateBuffers() const
{
    // Index Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(m_indices[0]), m_indices.data(), GL_STATIC_DRAW);

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITION_VB]);
    glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), m_positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Texture Coords
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXTURE_COORDS_VB]);
    glBufferData(GL_ARRAY_BUFFER, m_textureCoords.size() * sizeof(m_textureCoords[0]), m_textureCoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEXTURE_COORD_LOCATION);
    glVertexAttribPointer(TEXTURE_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), m_normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Bones
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[BONE_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_bones[0]) * m_bones.size(), &m_bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BONE_ID_LOCATION);
    glVertexAttribIPointer(BONE_ID_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), static_cast<const GLvoid*>(nullptr));
    glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
    glVertexAttribPointer(BONE_WEIGHT_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData),
                          reinterpret_cast<const GLvoid*>((MAX_NUM_BONES_PER_VERTEX * sizeof(int32_t))));

}

void ObjectMesh::checkOpenGLError(const std::string &location)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::string errorStr;
        switch (err)
        {
        case GL_INVALID_ENUM:
            errorStr = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorStr = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorStr = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            errorStr = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            errorStr = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            errorStr = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorStr = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            errorStr = "UNKNOWN_ERROR";
            break;
        }
        std::cerr << "OpenGL Error at " << location << ": " << errorStr << " (0x" << std::hex << err << ")" << std::endl;
    }
}