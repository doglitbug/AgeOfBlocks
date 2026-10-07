#pragma once

#include <format>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <string>

#include "../Texture.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

// These need to be mirrored in the shader
#define POSITION_LOCATION       0
#define TEXTURE_COORD_LOCATION  1
#define NORMAL_LOCATION         2
#define INSTANCED_LOCATIONS     3

#define COLOR_TEXTURE_UNIT GL_TEXTURE0 //?

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))
#define ARRAY_SIZE(a) (sizeof(a[0]) * a.size())
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices)

class StaticObject;

class ObjectInstanced
{
public:
    enum BUFFER_TYPE
    {
        INDEX_BUFFER = 0,
        POSITION_VB = 1,
        TEXTURE_COORDS_VB = 2,
        NORMAL_VB = 3,
        INSTANCED_POSITIONS = 4,
        NUMBER_BUFFERS = 5
    };
    ObjectInstanced() = default;
    ~ObjectInstanced() = default;

    bool LoadMesh(const std::string &filename);
    void loadData(const std::vector<glm::vec3> &p_objects);
    void Render(unsigned int meshIndex =-1) const;

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

    void RenderMesh(const InternalMesh &mesh) const;

    void LoadFromFile(const aiScene *pScene, const std::string &filename);
    void CountVerticesAndIndices(const aiScene *pScene, unsigned int &numberVertices, unsigned int &numberIndices);
    void ReserveSpace(unsigned int numberVertices, unsigned int numberIndices);

    void LoadMesh(uint meshIndex, const aiMesh *paiMesh);

    void LoadMaterials(const aiScene *xpScene, const std::string &filename);
    void PopulateBuffers() const;

    GLuint m_VAO;
    GLuint m_buffers[NUMBER_BUFFERS]{};
    std::vector<unsigned int> m_indices;
    std::vector<InternalMesh> m_meshes;
    std::vector<Texture *> m_textures;
    std::vector<glm::vec3> m_positions;
    std::vector<glm::vec2> m_textureCoords;
    std::vector<glm::vec3> m_normals;

    int m_numberInstances;

    Assimp::Importer importer;
    const aiScene* pScene;

    static void checkOpenGLError(const std::string& location);
};