#include "Mesh.h"

#include <assimp/postprocess.h>

#include <iostream>
#include <filesystem>

#include "glm/detail/type_quat.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#define DEBUG(x) std::cout << "Debug: " << x << std::endl;

// Helper to convert Assimp matrix to GLM matrix
glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

// Helper to convert GLM matrix back to Assimp matrix
aiMatrix4x4 glmToAiMatrix4x4(const glm::mat4& from) {
    aiMatrix4x4 to;
    to.a1 = from[0][0]; to.a2 = from[1][0]; to.a3 = from[2][0]; to.a4 = from[3][0];
    to.b1 = from[0][1]; to.b2 = from[1][1]; to.b3 = from[2][1]; to.b4 = from[3][1];
    to.c1 = from[0][2]; to.c2 = from[1][2]; to.c3 = from[2][2]; to.c4 = from[3][2];
    to.d1 = from[0][3]; to.d2 = from[1][3]; to.d3 = from[2][3]; to.d4 = from[3][3];
    return to;
}

glm::mat4 blenderCorrection = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

Mesh::Mesh()
{
    m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    m_scale = 1.0f;
}

bool Mesh::LoadMesh(const std::string &filename)
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);

    Assimp::Importer importer;

    const aiScene *pScene = importer.ReadFile(filename.c_str(), ASSIMP_LOAD_FLAGS);

    if (!pScene)
    {
        std::cerr << "Unable to parse model: " << filename << ": " << importer.GetErrorString() << std::endl;
        return false;
    }

    LoadFromScene(pScene, filename);

    PopulateBuffers();

    glBindVertexArray(0);
    return true;
}

void Mesh::Render(const unsigned int meshIndex) const
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

glm::mat4 Mesh::GetWorldMatrix() const
{
    constexpr auto identity = glm::mat4(1.0f);
    const auto translation = glm::translate(identity, m_position);
    const auto rotation = glm::mat4_cast(glm::quat(glm::radians(m_rotation)));

    // Remember T * R * S
    return translation * rotation * m_scale;
}

glm::mat3 Mesh::GetNormalMatrix() const
{
    const auto model = GetWorldMatrix();
    return glm::transpose(glm::inverse(model));
}

void Mesh::LoadFromScene(const aiScene *pScene, const std::string &filename)
{
    m_meshes.resize(pScene->mNumMeshes);
    m_textures.resize(pScene->mNumMaterials);

    unsigned int numberVertices = 0;
    unsigned int numberIndices = 0;
    unsigned int numberBones = 0;

    CountVerticesAndIndices(pScene, numberVertices, numberIndices);
    ReserveSpace(numberVertices, numberIndices);

    LoadAllMeshes(pScene);

    LoadMaterials(pScene, filename);
}

void Mesh::CountVerticesAndIndices(const aiScene *pScene, unsigned int &numberVertices, unsigned int &numberIndices)
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

void Mesh::ReserveSpace(const unsigned int numberVertices, unsigned int numberIndices)
{
    m_indices.reserve(numberIndices);
    m_positions.reserve(numberVertices);
    m_textureCoords.reserve(numberVertices);
    m_normals.reserve(numberVertices);
}

void Mesh::LoadAllMeshes(const aiScene *pScene)
{
    for (unsigned int i = 0; i < m_meshes.size(); i++)
    {
        const aiMesh *paiMesh = pScene->mMeshes[i];
        LoadMesh(paiMesh);
    }
}

void Mesh::LoadMesh(const aiMesh *paiMesh)
{
    // Populate vertex attribute vectors
    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
    {
        const aiVector3D &pPos = paiMesh->mVertices[i];
        const aiVector3D &pTextureCoords = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);
        const aiVector3D &pNormal = paiMesh->mNormals[i];//TODO Set to 0,1,0 if not present?

        // A -90 degree rotation around X changes: (X, Y, Z) -> (X, Z, -Y)
        m_positions.emplace_back(pPos.x, pPos.z, -pPos.y);
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
}

void Mesh::LoadAllBones(const aiMesh* pMesh)
{
    for (int i = 0; i < pMesh->mNumBones; i++)
    {
        LoadBone(i, pMesh->mBones[i]);
    }
}

void Mesh::LoadBone(int bone_index, const aiBone* pBone)
{
}

void Mesh::LoadMaterials(const aiScene *pScene, const std::string &filename)
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

void Mesh::PopulateBuffers()
{
    // Fix 1: Positions Size
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITION_VB]);
    glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), m_positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Fix 2: Texture Coords Size
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXTURE_COORDS_VB]);
    glBufferData(GL_ARRAY_BUFFER, m_textureCoords.size() * sizeof(m_textureCoords[0]), m_textureCoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEXTURE_COORD_LOCATION);
    glVertexAttribPointer(TEXTURE_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Fix 3: Normals Size
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), m_normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Fix 4: Index Buffer Size
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(m_indices[0]), m_indices.data(), GL_STATIC_DRAW);

}

void Mesh::checkOpenGLError(const std::string &location)
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