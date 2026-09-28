#include "PlayState.h"

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

    m_map = new map(64);
    m_map->generateMap();
    m_map->populateBuffers();
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

    UpdateDayNightCycle(deltaTime);

    if (App::get()->getInput()->getKeyDown(SDL_SCANCODE_X) && m_meshNumber < 21)
    {
        m_meshNumber++;
    }
    if (App::get()->getInput()->getKeyDown(SDL_SCANCODE_Z) && m_meshNumber > 0)
    {
        m_meshNumber--;
    }
}

void PlayState::render()
{
    RenderScene();
    drawHUD();
}

void PlayState::onExit()
{
    delete mCamera;
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
    //ImGui::Text("Camera position (x,y,z): %f %f %f", mCamera.m_position.x, mCamera.m_position.y, mCamera.m_position.z);
    ImGui::Text("Current language: %s", App::get()->getLanguage()->get("LANGUAGE").c_str());
    ImGui::End();

    // Rendering
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void PlayState::UpdateDayNightCycle(float deltaTime)
{
    // 1. Advance time and wrap around 1.0 (24 hours)
    //TODO Advance
    //timeOfDay += deltaTime / DAY_DURATION_SECONDS;
    if (timeOfDay > 1.0f) timeOfDay -= 1.0f;

    // 2. Calculate Sun Direction (Orbiting around the Z or X axis)
    float angle = (timeOfDay * 2.0f * 3.14159265f) - 1.57079632f;
    glm::vec3 sunPosition = {std::cos(angle) * 1.0f, std::sin(angle) * 1.0f, 0.0f}; // Sun rises/sets along X/Y plane

    // 3. Define Colors for Times of Day
    glm::vec3 nightColor = {0.05f, 0.05f, 0.1f};
    glm::vec3 sunriseColor = {0.9f, 0.4f, 0.2f};
    glm::vec3 dayColor = {1.0f, 1.0f, 0.9f};
    glm::vec3 sunsetColor = {0.8f, 0.3f, 0.3f};

    glm::vec3 nightAmbient = {0.02f, 0.02f, 0.05f};
    glm::vec3 dayAmbient = {0.2f, 0.2f, 0.25f};

    glm::vec3 currentLightColor = {0.0f, 0.0f, 0.0f};
    glm::vec3 currentAmbient = nightAmbient;

    // 4. Interpolate based on time ranges
    if (timeOfDay >= 0.0f && timeOfDay < 0.2f)
    {
        // Night to Dawn
        float t = timeOfDay / 0.2f;
        currentLightColor = glm::mix(nightColor, sunriseColor, t);
        currentAmbient = glm::mix(nightAmbient, dayAmbient, t);
    }
    else if (timeOfDay >= 0.2f && timeOfDay < 0.5f)
    {
        // Dawn to Noon
        float t = (timeOfDay - 0.2f) / 0.3f;
        currentLightColor = glm::mix(sunriseColor, dayColor, t);
        currentAmbient = dayAmbient;
    }
    else if (timeOfDay >= 0.5f && timeOfDay < 0.75f)
    {
        // Noon to Dusk
        float t = (timeOfDay - 0.5f) / 0.25f;
        currentLightColor = glm::mix(dayColor, sunsetColor, t);
        currentAmbient = dayAmbient;
    }
    else
    {
        // Dusk to Night
        float t = (timeOfDay - 0.75f) / 0.25f;
        currentLightColor = glm::mix(sunsetColor, nightColor, t);
        currentAmbient = glm::mix(dayAmbient, nightAmbient, t);
    }

    // Fade out light intensity completely when sun goes below horizon (under the ground)
    if (sunPosition.y < 0.0f)
    {
        currentLightColor = {0.0f, 0.0f, 0.0f}; // Only ambient light left at night
    }

    // 5. Send data to the shader programs
    mLightingStruct.ambientColor = currentAmbient;
    mLightingStruct.lightDirection = -sunPosition;
    mLightingStruct.lightColor = currentLightColor;
}

void PlayState::CompileShaders()
{
    m_3dShaderProgram.init();
    m_terrainShaderProgram.init();
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
    m_terrainShaderProgram.enable();
    m_map->render();
}