#pragma once
#include "BaseState.h"
#include "Camera.h"
#include "Camera.h"
#include "Texture.h"
#include "../World/CharacterShader.h"
#include "Terrain/map.h"
#include "Terrain/shaders/TerrainShader.h"
#include "ObjectMesh.h"

class PlayState: public BaseState
{
public:
    void onEnter() override;
    void update(float deltaTime) override;
    void render() override;
    void onExit() override;

    //TODO Private this
    int m_meshNumber = 0;

private:
    Camera* mCamera = nullptr;
    void drawHUD();
    void UpdateDayNightCycle(float deltaTime);


    CharacterShader m_3dShaderProgram;
    TerrainShader m_terrainShaderProgram;

    GLint gModelLocation;
    GLint gSamplerLocation;

    Texture *pTexture;

    void CompileShaders();
    void RenderScene();

    ObjectMesh m_playerObject;
    ObjectMesh m_NPC;

    map *m_map;

    GLuint ubos[2];

    struct viewStruct
    {
        glm::mat4 view;
        glm::mat4 projection;
    } mViewStruct;

    struct lightingStruct
    {
        glm::vec3 ambientColor;
        float _pad0;
        glm::vec3 lightDirection;
        float _pad1;
        glm::vec3 lightColor;
        float _pad2;
    } mLightingStruct;

    const float DAY_DURATION_SECONDS = 10.0f;
    float timeOfDay = 0.45f;
};
