#include "PlayState.h"

#include <chrono>

#include "Subsystems/App.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "glm/gtc/type_ptr.hpp"

void PlayState::onEnter()
{
    m_camera = new Camera();
    // TODO Store fov, near and far as variables?
    const auto resolution = App::get()->getSettings()->getResolution();
    m_camera->setPerspective(45.0f, resolution.x, resolution.y, 0.1f, 1000.0f);

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
    m_playerObject.m_position = glm::vec3(128.0f, 0.0f, 136.0f);

    // Only look this up once and save!
    gModelLocation = m_3dShaderProgram.getUniformLocation("model");
    // Texture Sampler
    gSamplerLocation = m_3dShaderProgram.getUniformLocation("gSampler");

    auto start = std::chrono::steady_clock::now();
    //TODO Chunking...10k size is too much for beast machine to display!
    m_map = new map(256);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // 5. Output the result using .count()
    std::cout << "Time taken by map generation section: "
        << duration.count() / 1000.0f << " seconds" << std::endl;


    App::get()->getSettings()->addObserver(this);

    m_lightingStruct = {
        .ambientColor = glm::vec3(0.18f, 0.24f, 0.35f),
        .lightDirection = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.4f)),
        .lightColor = glm::vec3(1.0f, 0.98f, 0.85f)
    };
}

void PlayState::update(float deltaTime)
{
    if (App::get()->getInput()->getMouseButtonState(mouseButtons::RIGHT))
    {
        App::get()->setMouseLock(true);
        m_camera->mouseLook(App::get()->getInput()->getMouseMovement(), deltaTime);
        m_playerObject.m_yaw = std::fmod(m_camera->getYaw() + 180.0f, 360.0f); //TODO Smooth this?
    }
    else
    {
        App::get()->setMouseLock(false);
    }

    //Player movement
    //TODO Check we aren't using a menu or something?

    m_camera->changeDistance(App::get()->getInput()->getMouseWheel());

    if (App::get()->getInput()->getAction(actions::MENU))
    {
        //TODO Save game request etc
        App::get()->quit();
    }


    //m_playerObject.getInput();// Get requested movement
    m_playerObject.m_position.y = m_map->getHeight(m_playerObject.m_position.x, m_playerObject.m_position.z);
    //m_playerObject.checkCollision();// Check collisions that prevent movement, eg wall sliding
    //m_playerObject.checkInteractions();// ??
    m_playerObject.update(deltaTime); // Animations

    m_map->update(deltaTime);
}

void PlayState::render()
{
    RenderScene();
    drawHUD();
}

void PlayState::onExit()
{
    App::get()->getSettings()->removeObserver(this);
    delete m_camera;
}

void PlayState::onNotify(const std::string& message, const MyType newValue)
{
    if (message == "RESOLUTION")
    {
        const auto resolution = std::get<glm::ivec2>(newValue);
        m_camera->setPerspective(45.0f, resolution.x, resolution.y, 0.1f, 1000.0f);
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
    ImGui::Text("Player position (x,y,z): %f %f %f", m_playerObject.m_position.x,
                m_playerObject.m_position.y,
                m_playerObject.m_position.z);

    ImGui::Text("Player yaw: %f", m_playerObject.m_yaw);

    ImGui::Text("Player velocity (x,y,z): %f %f %f", m_playerObject.m_velocity.x,
                m_playerObject.m_velocity.y,
                m_playerObject.m_velocity.z);

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
    // TODOEvery frame or only when it changes?
    m_viewStruct.view = m_camera->getViewMatrix(m_playerObject.m_position);
    m_viewStruct.projection = m_camera->getProjectionMatrix();

    glBindBuffer(GL_UNIFORM_BUFFER, ubos[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(viewStruct), &m_viewStruct);


    // Send lighting stuff to the GPU
    glBindBuffer(GL_UNIFORM_BUFFER, ubos[1]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lightingStruct), &m_lightingStruct);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_3dShaderProgram.enable();
    // Send the translation info to the GPU
    glUniformMatrix4fv(gModelLocation, 1, GL_FALSE, glm::value_ptr(m_playerObject.GetWorldMatrix()));

    std::vector<glm::mat4> transforms;
    //TODO Remove second parameter...or keep for poses?
    m_playerObject.GetBoneTransforms(transforms, m_playerObject.m_animationTime);

    for (uint i = 0; i < transforms.size(); i++)
    {
        //TODO Split up the shader program and use polymorphism
        m_3dShaderProgram.SetBoneTransform(i, transforms[i]);
    }
    m_playerObject.Render(2);

    // Draw the world
    m_map->render();
}
