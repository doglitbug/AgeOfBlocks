#pragma once

#include <format>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <string>
#include <SDL3/SDL_log.h>

#include "Utils.h"
#include "../../Texture.h"

// These need to be mirrored in the shader
#define POSITION_LOCATION       0
#define TEXTURE_COORD_LOCATION  1
#define NORMAL_LOCATION         2
#define BONE_ID_LOCATION        3
#define BONE_WEIGHT_LOCATION    4

#define COLOR_TEXTURE_UNIT GL_TEXTURE0 //?

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))
#define ARRAY_SIZE(a) (sizeof(a[0]) * a.size())
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices)

class ObjectAnimated
{
public:
    enum BUFFER_TYPE
    {
        INDEX_BUFFER = 0,
        POSITION_VB = 1,
        TEXTURE_COORDS_VB = 2,
        NORMAL_VB = 3,
        BONE_VB = 4,
        NUMBER_BUFFERS = 5
    };

    ObjectAnimated();
    ~ObjectAnimated() = default;

    bool LoadMesh(const std::string &filename);
    void Render(unsigned int meshIndex) const;

    // TODO move back to private and add accessor methods
    glm::vec3 m_position{};
    glm::vec3 m_rotation{};
    float m_scale = 1.0f;
    float m_animationTime = 0.0f;

    [[nodiscard]] glm::mat4 GetWorldMatrix() const;

    void GetBoneTransforms(std::vector<glm::mat4> &boneTransforms, float animationTime);
private:
    #define MAX_NUM_BONES_PER_VERTEX 4

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

    struct VertexBoneData
    {
        uint BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
        float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

        VertexBoneData() = default;

        void AddBoneData(const uint BoneID, const float Weight)
        {
            for (uint i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(BoneIDs) ; i++) {
                if (Weights[i] == 0.0) {
                    BoneIDs[i] = BoneID;
                    Weights[i] = Weight;
                    //printf("Adding bone %d weight %f at index %i\n", BoneID, Weight, i);
                    return;
                }
            }

            // should never get here - more bones than we have space for
            //assert(0);
            SDL_Log("Shit"); // BoneID 33, weight 0 comes here once per villager file load
        }
    };

    struct BoneInfo
    {
        glm::mat4 offsetMatrix{};
        glm::mat4 finalTransformation{};

        explicit BoneInfo(const aiMatrix4x4 &offset)
        {
            const auto glm = aiMatrix4x4ToGlm(offset);
            offsetMatrix = glm;
            finalTransformation = {0};
        }
    };
    std::vector<BoneInfo> m_BoneInfo;

    void LoadFromFile(const aiScene *pScene, const std::string &filename);
    void CountVerticesAndIndices(const aiScene *pScene, unsigned int &numberVertices, unsigned int &numberIndices);
    void ReserveSpace(unsigned int numberVertices, unsigned int numberIndices);

    void LoadMesh(uint meshIndex, const aiMesh *paiMesh);

    void LoadBone(uint meshIndex, const aiBone* pBone);
    int GetBoneId(const aiBone* pBone);

    void ReadNodeHierarchy(float animationTimeTicks, const aiNode* pNode, const glm::mat4 &parentTransform);
    void CalculateInterpolatedScaling(aiVector3D &out, float animationTimeTicks, const aiNodeAnim* pNodeAnim);
    unsigned int FindScaling(float animationTimeTicks, const aiNodeAnim* pNodeAnim);
    void CalculateInterpolatedRotation(aiQuaternion &out, float animationTimeTicks, const aiNodeAnim* pNodeAnim);
    unsigned int FindRotation(float animationTimeTicks, const aiNodeAnim* pNodeAnim);
    void CalculateInterpolatedPosition(aiVector3D &out, float animationTimeTicks, const aiNodeAnim* pNodeAnim);
    unsigned int FindPosition(float animationTimeTicks, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName);

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
    std::vector<VertexBoneData> m_bones;

    std::map<std::string,uint> m_BoneNameToIndexMap;

    Assimp::Importer importer;
    const aiScene* pScene;

    static void checkOpenGLError(const std::string& location);
};