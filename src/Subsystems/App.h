#pragma once

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "Settings.h"
#include "InputSystem.h"
#include "Camera.h"
#include "ObjectMesh.h"
#include "../World/CharacterShader.h"
#include "Terrain/map.h"
#include "Terrain/shaders/TerrainShader.h"

class App : public IObserver
{
public:
    static App *get()
    {
        static App me;
        return &me;
    };

    // Singleton
    App(App &other) = delete;
    void operator=(const App &) = delete;

    void onNotify(const std::string &message, MyType newValue) override;

    void init();
    void handleEvents();
    void update(float deltaTime);
    void render();

    void toggleMouseLock();
    void setResolution(int width, int height, bool resize = false);
    [[nodiscard]] bool running() const { return m_bRunning; }
    void quit() { m_bRunning = false; }

    [[nodiscard]] Settings *getSettings() const { return m_pSettings; }
    [[nodiscard]] InputSystem *getInput() const { return m_pInput; }

    int m_meshNumber = 0;
private:
    App() = default;
    ~App();

    void CompileShaders();
    void RenderScene();

    SDL_Window *m_pWindow;
    Settings *m_pSettings;
    InputSystem *m_pInput;
    Camera mCamera{};

    SDL_GLContext glContext;

    CharacterShader m_3dShaderProgram;
    TerrainShader m_terrainShaderProgram;

    GLint gModelLocation;

    GLint gSamplerLocation;

    Texture *pTexture;

    bool m_bRunning;
    bool m_mouseLocked;

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
    float timeOfDay = 0.5f;
    void UpdateDayNightCycle(float deltaTime);
};
