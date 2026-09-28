#pragma once

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Settings.h"
#include "InputSystem.h"
#include "Language.h"
#include "States/StateMachine.h"

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
    bool running() const { return m_bRunning; }
    void quit() { m_bRunning = false; }

    Settings *getSettings() const { return m_pSettings; }
    InputSystem *getInput() const { return m_pInput; }
    Language *getLanguage() const { return m_pLanguage; }
    StateMachine* getStateMachine() const { return m_pStateMachine; }

private:
    App() = default;
    ~App() override;

    SDL_Window *m_pWindow;
    SDL_GLContext glContext;
    Settings *m_pSettings;
    InputSystem *m_pInput;
    Language *m_pLanguage;
    StateMachine* m_pStateMachine;

    bool m_bRunning;
    bool m_mouseLocked;
};
