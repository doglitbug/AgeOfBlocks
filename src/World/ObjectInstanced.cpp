#include "ObjectInstanced.h"

#include <assimp/postprocess.h>

#include <iostream>
#include <filesystem>
#include <SDL3/SDL_log.h>

#include "glm/detail/type_quat.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#define DEBUG(x) std::cout << "Debug: " << x << std::endl;

ObjectInstanced::ObjectInstanced()
{
    m_scale = 1.0f;
}

bool ObjectInstanced::LoadMesh(const std::string &filename)
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

void ObjectInstanced::Render(const unsigned int meshIndex) const
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

void ObjectInstanced::LoadFromFile(const aiScene *pScene, const std::string &filename)
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

void ObjectInstanced::CountVerticesAndIndices(const aiScene *pScene, unsigned int &numberVertices, unsigned int &numberIndices)
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

void ObjectInstanced::ReserveSpace(const unsigned int numberVertices, const unsigned int numberIndices)
{
    m_indices.reserve(numberIndices);
    m_positions.reserve(numberVertices);
    m_textureCoords.reserve(numberVertices);
    m_normals.reserve(numberVertices);
}

void ObjectInstanced::LoadMesh(const uint meshIndex, const aiMesh *paiMesh)
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
}

void ObjectInstanced::LoadMaterials(const aiScene *xpScene, const std::string &filename)
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

void ObjectInstanced::PopulateBuffers() const
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

}

void ObjectInstanced::checkOpenGLError(const std::string &location)
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