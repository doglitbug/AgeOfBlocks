#include <iostream>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "App.h"

#include "States/MainMenuState.h"
#include "States/PlayState.h"

void App::onNotify(const std::string& message, const MyType newValue)
{
    if (message == "RESOLUTION")
    {
        const glm::ivec2 screenResolution = std::get<glm::ivec2>(newValue);
        glViewport(0, 0, screenResolution.x, screenResolution.y);
    }
}

void App::init()
{
    m_pSettings = new Settings();
    m_pSettings->addObserver(this);

    m_pLanguage = new Language();
    m_pSettings->addObserver(m_pLanguage);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        SDL_Log("SDL Init fail");
        exit(1);
    }

    // Init the window
    int flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    if (m_pSettings->getFullScreen())
        flags |= SDL_WINDOW_FULLSCREEN;

    const glm::ivec2 screenResolution = m_pSettings->getResolution(); // Used in OPENGL further down

    m_pWindow = SDL_CreateWindow(m_pLanguage->get("MENU_NAME").c_str(), screenResolution.x, screenResolution.y, flags);
    if (m_pWindow == nullptr)
    {
        SDL_Log("Window Creation Error: %s", SDL_GetError());
        exit(1);
    }

    // OpenGL Stuff version 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    glContext = SDL_GL_CreateContext(m_pWindow);
    SDL_GL_MakeCurrent(m_pWindow, glContext);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
    {
        SDL_Log("Failed to link OpenGL extensions via GLAD");
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Create input subsystem
    m_pInput = new InputSystem();
    // TODO Move these to constructor
    m_pInput->initializeGamepads();
    m_pInput->initializeMouse();


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(m_pWindow, glContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    //Create state machine and populate
    m_pStateMachine = new StateMachine();

    m_pStateMachine->registerState("MAINMENU", new MainMenuState());
    m_pStateMachine->registerState("PLAY", new PlayState());

    m_bRunning = true;
}

void App::handleEvents()
{
    SDL_Event event;
    //TODO Check this is needed
    m_pInput->resetMouseMovement();

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            quit();
            return;
        case SDL_EVENT_WINDOW_RESIZED:
            m_pSettings->setResolution(event.window.data1, event.window.data2);
            return;
        default:
            m_pInput->update(event);
            break;
        }
    }
}

void App::update(const float deltaTime)
{
    m_pStateMachine->update(deltaTime);
}

void App::render()
{
    glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_pStateMachine->render();
    SDL_GL_SwapWindow(m_pWindow);
}

void App::setMouseLock(const bool newState)
{
    m_mouseLocked = newState;
    SDL_SetWindowRelativeMouseMode(m_pWindow, newState);
}

App::~App()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(m_pWindow);
    SDL_Quit();
}