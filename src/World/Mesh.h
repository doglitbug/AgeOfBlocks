#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <string>

#include "Texture.h"

// These need to be mirrored in the shader
#define POSITION_LOCATION 0
#define TEXTURE_COORD_LOCATION 1
#define NORMAL_LOCATION 2

#define COLOR_TEXTURE_UNIT GL_TEXTURE0 //?

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))
#define ARRAY_SIZE(a) (sizeof(a[0]) * a.size())
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices)
enum BUFFER_TYPE
{
    INDEX_BUFFER = 0,
    POSITION_VB = 1,
    TEXTURE_COORDS_VB = 2,
    NORMAL_VB = 3,
    WVP_MAT_VB = 4,   // ??
    WORLD_MAT_VB = 5, // ??
    NUMBER_BUFFERS = 6
};

class Mesh
{
public:
    Mesh();
    ~Mesh() = default;

    bool LoadMesh(const std::string &filename);
    void Render(unsigned int meshIndex) const;

    // TODO move back to private and add accessor methods
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    float m_scale;

    glm::mat4 GetWorldMatrix() const;
private:
    struct InternalMesh
    {
        InternalMesh()
        {
            numberIndices = 0;
            startingVertex = 0;
            startingIndex = 0;
            materialIndex = -1;
        };
        unsigned int numberIndices;
        unsigned int startingVertex;
        unsigned int startingIndex;
        unsigned int materialIndex;
    };
    void LoadFromScene(const aiScene *pScene, const std::string &filename);
    void CountVerticesAndIndices(const aiScene *pScene, unsigned int &numberVertices, unsigned int &numberIndices);
    void ReserveSpace(unsigned int numberVertices, unsigned int numberIndices);
    void LoadAllMeshes(const aiScene *pScene);
    void LoadMesh(const aiMesh *paiMesh);
    void LoadMaterials(const aiScene *pScene, const std::string &filename);
    void PopulateBuffers();

    GLuint m_VAO;
    GLuint m_buffers[NUMBER_BUFFERS];
    std::vector<unsigned int> m_indices;
    std::vector<InternalMesh> m_meshes;
    std::vector<Texture *> m_textures;
    std::vector<glm::vec3> m_positions;
    std::vector<glm::vec2> m_textureCoords;
    std::vector<glm::vec3> m_normals;



    void checkOpenGLError(const std::string& location);
};