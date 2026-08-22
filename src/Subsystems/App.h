#pragma once

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "Settings.h"
#include "InputSystem.h"
#include "Camera.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#define HELLO std::cout << "Hello" << std::endl;
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
    void render() const;
    void update(float deltaTime);
    void handleEvents();

    void toggleMouseLock();
    bool running() const { return m_bRunning; }
    void quit() { m_bRunning = false; }

    Settings *getSettings() const { return m_pSettings; }
    InputSystem *getInput() const { return m_pInput; }

private:
    App() = default;
    ~App();

    void CompileShaders();
    void RenderScene() const;

    SDL_Window *m_pWindow;
    Settings *m_pSettings;
    InputSystem *m_pInput;
    Camera mCamera;

    SDL_GLContext glContext;

    ShaderProgram m_3dShaderProgram;
    GLint gTranslateLocation;
    GLint gCameraLocation;
    GLint gSamplerLocation;

    Texture *pTexture;

    float gScale = 0.0f;
    glm::mat4 gTranslate;
    glm::mat4 gPerspective;
    float gDelta = 0.5f;
    bool m_bRunning;
    bool m_mouseLocked;

    Mesh meh;
};
