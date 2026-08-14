#include <iostream>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#include "App.h"
bool readFileToString(const std::string &filePath, std::string &output)
{
    std::ifstream inputFile(filePath, std::ios::in | std::ios::binary);
    if (!inputFile.is_open())
    {
        std::cerr << "Unable to open file" << std::endl;
        return false;
    }
    std::ostringstream streamBuffer;
    streamBuffer << inputFile.rdbuf();
    output = streamBuffer.str();
    return true;
}
void App::onNotify(const std::string &message, MyType newValue)
{
    if (message == "RESOLUTION")
    {
        glm::ivec2 screenResolution = std::get<glm::ivec2>(newValue);
        // TODO Store fov, near and far as variables?
        mCamera.setPerspective(45.0f, screenResolution.x, screenResolution.y, 0.1f, 100.0f);
        glViewport(0, 0, screenResolution.x, screenResolution.y);
    }
}
void App::init()
{
    m_pSettings = new Settings();

    // Attempt to initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        std::cerr << "SDL Init fail" << std::endl;
        exit(1);
    }

    // Init the window
    int flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    if (m_pSettings->getFullScreen())
        flags |= SDL_WINDOW_FULLSCREEN;

    glm::ivec2 screenResolution = App::getSettings()->getResolution(); // Used in OPENGL further down

    m_pWindow = SDL_CreateWindow("Age of Blocks", screenResolution.x, screenResolution.y, flags);
    if (m_pWindow == nullptr)
    {
        std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // OpenGL Stuff
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GLContext glContext = SDL_GL_CreateContext(m_pWindow);
    SDL_GL_MakeCurrent(m_pWindow, glContext);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to link OpenGL extensions via GLAD" << std::endl;
        exit(-1);
    }

    // TODO Get an observer for screen res changing and reset these two lines
    mCamera.setPerspective(45.0f, screenResolution.x, screenResolution.y, 0.1f, 100.0f);
    glViewport(0, 0, screenResolution.x, screenResolution.y);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    CompileShaders();

    meh.LoadMesh("assets/models/villager.gltf");

    // Only look this up once and save!
    gTranslateLocation = glGetUniformLocation(shaderProgram, "gTranslate");
    if (gTranslateLocation == -1)
    {
        std::cerr << "Failed to find gTranslate" << std::endl;
    }

    // Set up Camera
    gCameraLocation = glGetUniformLocation(shaderProgram, "gCamera");
    if (gCameraLocation == -1)
    {
        std::cerr << "Failed to find gCamera" << std::endl;
    }

    // Texture Sampler
    gSamplerLocation = glGetUniformLocation(shaderProgram, "gSampler");
    if (gSamplerLocation == -1)
    {
        std::cerr << "Failed to find gSampler" << std::endl;
    }

    // Create input subsystem
    m_pInput = new InputSystem();
    // TODO Move these to constructor
    m_pInput->initializeGamepads();
    m_pInput->initializeMouse();

    SDL_WarpMouseInWindow(m_pWindow, screenResolution.x / 2, screenResolution.y / 2);
    SDL_SetWindowRelativeMouseMode(m_pWindow, true);

    m_bRunning = true;
}

void App::render()
{
    glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderScene();
    SDL_GL_SwapWindow(m_pWindow);
}

void App::update(float deltaTime)
{
    gScale += deltaTime;

    m_pInput->update();

    // Do Camera movement, later on this will be moving a player object that the camera is attached to
    mCamera.Move(m_pInput->getMovement() * deltaTime * 10.0f);
    // TODO Mouse movement would need to rotate the player object too.
    mCamera.MouseLook(m_pInput->getMouseMovement() * deltaTime);
}

void App::handleEvents()
{
    if (m_pInput->getAction(actions::MENU))
    {
        toggleMouseLock();
    }
}

void App::toggleMouseLock()
{
    m_mouseLocked = !m_mouseLocked;
    SDL_SetWindowRelativeMouseMode(m_pWindow, m_mouseLocked);
}

App::~App()
{
    glDeleteProgram(shaderProgram);

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(m_pWindow);
    SDL_Quit();
}
void App::CompileShaders()
{
    shaderProgram = glCreateProgram();

    if (shaderProgram == 0)
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    std::string vs, fs;

    if (!readFileToString("src/World/shaders/vertex.glsl", vs))
    {
        exit(1);
    };

    AddShader(shaderProgram, vs.c_str(), GL_VERTEX_SHADER);

    if (!readFileToString("src/World/shaders/fragment.glsl", fs))
    {
        exit(1);
    };

    AddShader(shaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0)
    {
        glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glValidateProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success)
    {
        glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(shaderProgram);
}
void App::RenderScene()
{
    // Send the camera stuff to the GPU
    auto cameraMatrix = mCamera.getViewMatrix();
    glUniformMatrix4fv(gCameraLocation, 1, GL_FALSE, glm::value_ptr(cameraMatrix));

    // Send the translation info to the GPU
    auto identity = glm::mat4(1.0f);
    identity = glm::scale(identity, glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(gTranslateLocation, 1, GL_FALSE, glm::value_ptr(identity));

    meh.Render(1);
}

void App::AddShader(GLuint ShaderProgram, const char *pShaderText, GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0)
    {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(1);
    }

    const GLchar *p[1];
    p[0] = pShaderText;

    GLint Lengths[1];
    Lengths[0] = (GLint)strlen(pShaderText);

    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);
}