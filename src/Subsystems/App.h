#pragma once

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "Settings.h"
#include "InputSystem.h"
#include "Camera.h"
#include "Object.h"
#include "ShaderProgram.h"

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
    void render() const;

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
    void RenderScene() const;

    SDL_Window *m_pWindow;
    Settings *m_pSettings;
    InputSystem *m_pInput;
    Camera mCamera{};

    SDL_GLContext glContext;

    ShaderProgram m_3dShaderProgram;
    GLint gModelLocation;
    GLint gNormalLocation;
    GLint gCameraLocation;
    GLint gSamplerLocation;

    Texture *pTexture;

    bool m_bRunning;
    bool m_mouseLocked;

    Object m_playerObject;
    Object m_NPC;
};
