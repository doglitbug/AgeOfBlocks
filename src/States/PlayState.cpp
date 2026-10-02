#include "PlayState.h"

#include <chrono>
#include <SDL3/SDL_log.h>

#include "App.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "glm/gtc/type_ptr.hpp"

void PlayState::onEnter()
{
    mCamera = new Camera();
    // TODO Store fov, near and far as variables?
    const auto resolution = App::get()->getSettings()->getResolution();
    mCamera->setPerspective(45.0f, resolution.x, resolution.y, 0.1f, 1000.0f);

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

    auto start = std::chrono::steady_clock::now();
    m_map = new map(64);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // 5. Output the result using .count()
    std::cout << "Time taken by code section: "
              << duration.count() << " microseconds" << std::endl;


    App::get()->getSettings()->addObserver(this);
    App::get()->setMouseLock(true);

    mLightingStruct = {
        .ambientColor   = glm::vec3(0.18f, 0.24f, 0.35f),
        .lightDirection = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.4f)),
        .lightColor     = glm::vec3(1.0f, 0.98f, 0.85f)
    };
}

void PlayState::update(float deltaTime)
{
    m_playerObject.m_rotation.y += deltaTime * 50;
    //TODO Wrap around, possibly add to a object.update(deltaTime)
    m_playerObject.m_animationTime += deltaTime;

    // Do Camera movement, later on this will be moving a player object that the camera is attached to
    mCamera->move(App::get()->getInput()->getMovement() * deltaTime * 10.0f);
    // TODO Mouse movement would need to rotate the player object too.
    mCamera->mouseLook(App::get()->getInput()->getMouseMovement() * deltaTime);

    if (App::get()->getInput()->getKeyDown(SDL_SCANCODE_X) && m_meshNumber < 21)
    {
        m_meshNumber++;
    }
    if (App::get()->getInput()->getKeyDown(SDL_SCANCODE_Z) && m_meshNumber > 0)
    {
        m_meshNumber--;
    }

    //Hack for player height
    mCamera->m_position.y = m_map->getHeight(mCamera->m_position.x, mCamera->m_position.z)+1.8f;
}

void PlayState::render()
{
    RenderScene();
    drawHUD();
}

void PlayState::onExit()
{
    App::get()->getSettings()->removeObserver(this);
    delete mCamera;
}

void PlayState::onNotify(const std::string& message, const MyType newValue)
{
    if (message == "RESOLUTION"){
        auto resolution = std::get<glm::ivec2>(newValue);
        mCamera->setPerspective(45.0f, resolution.x, resolution.y, 0.1f, 1000.0f);
    }
}

void PlayState::drawHUD()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Create a simple window
    ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
    ImGui::Begin("Debug menu");
    //ImGui::Text("Current time of day: %f", timeOfDay);
    ImGui::Text("Camera position (x,y,z): %f %f %f", mCamera->m_position.x, mCamera->m_position.y, mCamera->m_position.z);
    ImGui::Text("Current language: %s", App::get()->getLanguage()->get("LANGUAGE").c_str());
    ImGui::End();

    // Rendering
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void PlayState::CompileShaders()
{
    m_3dShaderProgram.init();
}

void PlayState::RenderScene()
{
    // Send the camera stuff to the GPU
    mViewStruct.view = mCamera->getViewMatrix();
    mViewStruct.projection = mCamera->getProjectionMatrix();

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

    for (uint i = 0; i < transforms.size(); i++)
    {
        //TODO Split up the shader program and use polymorphism
        m_3dShaderProgram.SetBoneTransform(i, transforms[i]);
    }
    m_playerObject.Render(m_meshNumber);

    glUniformMatrix4fv(gModelLocation, 1, GL_FALSE, glm::value_ptr(m_NPC.GetWorldMatrix()));
    m_NPC.Render(21);


    // Draw the world
    m_map->render();
}