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
    glFrontFace(GL_CW); // This doesn't seem to affect the models, check for walls/terrain?
    glCullFace(GL_BACK);



    glGenBuffers(2, ubos);

    // 2. Configure the first UBO: viewUniform
    glBindBuffer(GL_UNIFORM_BUFFER, ubos[0]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(viewStruct), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubos[0]); // Attach to Binding Point 0

    // 3. Configure the second UBO: lightingUniform
    glBindBuffer(GL_UNIFORM_BUFFER, ubos[1]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(lightingStruct), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubos[1]); // Attach to Binding Point 1

    // 4. Unbind the buffer target
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    CompileShaders();

    m_playerObject.LoadMesh("assets/models/villager.gltf");
    m_playerObject.m_position = glm::vec3(5.0f, 0.0f, 5.0f);

    m_NPC.LoadMesh("assets/models/villager.gltf");
    m_NPC.m_position = glm::vec3(2.0f, 0.0f, 2.0f);

    // Only look this up once and save!
    gModelLocation = m_3dShaderProgram.getUniformLocation("model");
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


    UpdateDayNightCycle(deltaTime);
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
    m_terrainShaderProgram.init();
}

void App::RenderScene()
{
    // Send the camera stuff to the GPU
    mViewStruct.view = mCamera.getViewMatrix();
    mViewStruct.projection = mCamera.getProjectionMatrix();

    glBindBuffer(GL_UNIFORM_BUFFER, ubos[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(viewStruct), &mViewStruct);

    // Send lighting stuff to the GPU

    glBindBuffer(GL_UNIFORM_BUFFER, ubos[1]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lightingStruct), &mLightingStruct);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);





    m_3dShaderProgram.enable();
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
    m_map->render();

    // TODO switch to 2d shader program and render GUI (or put in another function)
    //SDL_Log("Camera position (x,y,z): (%f,%f,%f)", mCamera.m_position.x, mCamera.m_position.y, mCamera.m_position.z);
    SDL_Log("Current time: (%f)", timeOfDay);
}

void App::UpdateDayNightCycle(float deltaTime) {
    // 1. Advance time and wrap around 1.0 (24 hours)
    //TODO Advance
    timeOfDay += deltaTime / DAY_DURATION_SECONDS;
    if (timeOfDay > 1.0f) timeOfDay -= 1.0f;

    // 2. Calculate Sun Direction (Orbiting around the Z or X axis)
    float angle = (timeOfDay * 2.0f * 3.14159265f) - 1.57079632f;
    glm::vec3 sunPosition = { std::cos(angle), std::sin(angle), 0.0f }; // Sun rises/sets along X/Y plane

    // 3. Define Colors for Times of Day
    glm::vec3 nightColor   = { 0.05f, 0.05f, 0.1f };
    glm::vec3 sunriseColor = { 0.9f, 0.4f, 0.2f };
    glm::vec3 dayColor     = { 1.0f, 1.0f, 0.9f };
    glm::vec3 sunsetColor  = { 0.8f, 0.3f, 0.3f };

    glm::vec3 nightAmbient   = { 0.02f, 0.02f, 0.05f };
    glm::vec3 dayAmbient     = { 0.2f, 0.2f, 0.25f };

    glm::vec3 currentLightColor = { 0.0f, 0.0f, 0.0f };
    glm::vec3 currentAmbient = nightAmbient;

    // 4. Interpolate based on time ranges
    if (timeOfDay >= 0.0f && timeOfDay < 0.2f) { // Night to Dawn
        float t = timeOfDay / 0.2f;
        currentLightColor = glm::mix(nightColor, sunriseColor, t);
        currentAmbient = glm::mix(nightAmbient, dayAmbient, t);
    }
    else if (timeOfDay >= 0.2f && timeOfDay < 0.5f) { // Dawn to Noon
        float t = (timeOfDay - 0.2f) / 0.3f;
        currentLightColor = glm::mix(sunriseColor, dayColor, t);
        currentAmbient = dayAmbient;
    }
    else if (timeOfDay >= 0.5f && timeOfDay < 0.75f) { // Noon to Dusk
        float t = (timeOfDay - 0.5f) / 0.25f;
        currentLightColor = glm::mix(dayColor, sunsetColor, t);
        currentAmbient = dayAmbient;
    }
    else { // Dusk to Night
        float t = (timeOfDay - 0.75f) / 0.25f;
        currentLightColor = glm::mix(sunsetColor, nightColor, t);
        currentAmbient = glm::mix(dayAmbient, nightAmbient, t);
    }

    // Fade out light intensity completely when sun goes below horizon (under the ground)
    if (sunPosition.y < 0.0f) {
        currentLightColor = { 0.0f, 0.0f, 0.0f }; // Only ambient light left at night
    }

    // 5. Send data to the shader programs
    mLightingStruct.ambientColor = currentAmbient;
    mLightingStruct.lightDirection = -sunPosition;
    mLightingStruct.lightColor = currentLightColor;
}