#pragma once
#include "BaseState.h"
#include "Camera.h"
#include "Texture.h"
#include "World/Characters/shaders/CharacterShader.h"
#include "World/map.h"
#include "World/Characters/ObjectAnimated.h"
#include "Observers.h"

class PlayState: public BaseState, IObserver
{
public:
    void onEnter() override;
    void update(float deltaTime) override;
    void render() override;
    void onExit() override;

    void onNotify(const std::string &message, MyType newValue) override;

private:
    Camera* mCamera = nullptr;
    void toggleMouseLock();
    void drawHUD();

    CharacterShader m_3dShaderProgram;

    GLint gModelLocation;
    GLint gSamplerLocation;

    Texture *pTexture;

    void CompileShaders();
    void RenderScene();

    ObjectAnimated m_playerObject;

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
};
