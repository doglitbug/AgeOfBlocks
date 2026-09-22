#include <iostream>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "App.h"

void App::onNotify(const std::string& message, const MyType newValue)
{
    if (message == "RESOLUTION")
    {
        const glm::ivec2 screenResolution = std::get<glm::ivec2>(newValue);
        setResolution(screenResolution.x, screenResolution.y, true);
    }
}

void App::init()
{
    m_pSettings = new Settings();
    m_pSettings->addObserver(this);

    // Attempt to initialize SDL
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

    m_pWindow = SDL_CreateWindow("Age of Blocks", screenResolution.x, screenResolution.y, flags);
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

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        SDL_Log("Failed to link OpenGL extensions via GLAD");
        exit(1);
    }

    setResolution(screenResolution.x, screenResolution.y);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    CompileShaders();

    m_playerObject.LoadMesh("assets/models/villager.gltf");
    m_NPC.LoadMesh("assets/models/villager.gltf");
    m_NPC.m_position = glm::vec3(2.0f, 0.0f, 0.0f);

    // Only look this up once and save!
    gModelLocation = m_3dShaderProgram.getUniformLocation("model");

    // Set up Camera
    gCameraViewLocation = m_3dShaderProgram.getUniformLocation("view");
    gCameraProjectionLocation = m_3dShaderProgram.getUniformLocation("projection");

    // Texture Sampler
    gSamplerLocation = m_3dShaderProgram.getUniformLocation("gSampler");

    // Create input subsystem
    m_pInput = new InputSystem();
    // TODO Move these to constructor
    m_pInput->initializeGamepads();
    m_pInput->initializeMouse();

    SDL_WarpMouseInWindow(m_pWindow, screenResolution.x / 2, screenResolution.y / 2);
    SDL_SetWindowRelativeMouseMode(m_pWindow, true);


    m_map = new map(10);
    m_map->generateMap();
    m_map->populateBuffers();

    m_bRunning = true;
}

void App::handleEvents()
{
    SDL_Event event;
    m_pInput->resetMouseMovement();

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            quit();
            return;
        case SDL_EVENT_WINDOW_RESIZED:
            setResolution(event.window.data1, event.window.data2);
            //TODO Tell the settings so it can be saved?
            return;
        default:
            m_pInput->update(event);
            break;
        }
    }

    if (m_pInput->getAction(actions::MENU))
    {
        toggleMouseLock();
    }
}

void App::update(const float deltaTime)
{
    //m_playerObject.m_rotation.y += deltaTime * 50;
    //TODO Wrap around, possibly add to a object.update(deltaTime)
    m_playerObject.m_animationTime += deltaTime;

    // Do Camera movement, later on this will be moving a player object that the camera is attached to
    mCamera.move(m_pInput->getMovement() * deltaTime * 10.0f);
    // TODO Mouse movement would need to rotate the player object too.
    mCamera.mouseLook(m_pInput->getMouseMovement() * deltaTime);
}

void App::render()
{
    glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderScene();
    SDL_GL_SwapWindow(m_pWindow);
}

void App::toggleMouseLock()
{
    m_mouseLocked = !m_mouseLocked;
    SDL_SetWindowRelativeMouseMode(m_pWindow, m_mouseLocked);
}

void App::setResolution(const int width, const int height, const bool resize)
{
    // Change window size?
    if (resize)
    {
        SDL_Log("Window resize");
    }
    // TODO Store fov, near and far as variables?
    mCamera.setPerspective(45.0f, width, height, 0.1f, 100.0f);
    glViewport(0, 0, width, height);
}

App::~App()
{
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(m_pWindow);
    SDL_Quit();
}

void App::CompileShaders()
{
    m_3dShaderProgram.init();
    std::string vs, fs;

    if (!Utils::readFileToString("src/World/shaders/vertex.glsl", vs))
    {
        exit(1);
    };

    m_3dShaderProgram.addShader(GL_VERTEX_SHADER, vs.c_str());

    if (!Utils::readFileToString("src/World/shaders/fragment.glsl", fs))
    {
        exit(1);
    };

    m_3dShaderProgram.addShader(GL_FRAGMENT_SHADER, fs.c_str());

    m_3dShaderProgram.finalise();

    m_terrainShaderProgram.init();
}

void App::RenderScene()
{
    glDisable(GL_CULL_FACE);
    m_3dShaderProgram.enable();

    // Send the camera stuff to the GPU
    auto cameraView = mCamera.getViewMatrix();
    glUniformMatrix4fv(gCameraViewLocation, 1, GL_FALSE, glm::value_ptr(cameraView));
    auto cameraProjection = mCamera.getProjectionMatrix();
    glUniformMatrix4fv(gCameraProjectionLocation, 1, GL_FALSE, glm::value_ptr(cameraProjection));

    // Send the translation info to the GPU
    // TODO Move to objects render?
    glUniformMatrix4fv(gModelLocation, 1, GL_FALSE, glm::value_ptr(m_playerObject.GetWorldMatrix()));

    std::vector<glm::mat4> transforms;
    //TODO Remove second parameter...or keep for poses?
    m_playerObject.GetBoneTransforms(transforms, m_playerObject.m_animationTime);

    for (uint i = 0 ; i < transforms.size() ; i++) {
        //TODO Split up the shader program and use polymorphism
        m_3dShaderProgram.SetBoneTransform(i, transforms[i]);
    }
    m_playerObject.Render(m_meshNumber);

    glUniformMatrix4fv(gModelLocation, 1, GL_FALSE, glm::value_ptr(m_NPC.GetWorldMatrix()));
    m_NPC.Render(21);



    // Draw the world
    m_terrainShaderProgram.enable();
    //TODO Pass camera to render function, or should it be cached?
    glUniformMatrix4fv(m_terrainShaderProgram.m_cameraViewLocation, 1, GL_FALSE, glm::value_ptr(cameraView));
    glUniformMatrix4fv(m_terrainShaderProgram.m_cameraProjectionLocation, 1, GL_FALSE, glm::value_ptr(cameraProjection));
    m_map->render();

    // TODO switch to 2d shader program and render GUI (or put in another function)
    SDL_Log("Camera position (x,y,z): (%f,%f,%f)", mCamera.m_position.x, mCamera.m_position.y, mCamera.m_position.z);
}
