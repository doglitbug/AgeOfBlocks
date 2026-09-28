#include "MainMenuState.h"

#include "App.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

#define LABEL(x) App::get()->getLanguage()->get(x).c_str()

void MainMenuState::onEnter()
{

}

void MainMenuState::update(float deltaTime)
{

}

void MainMenuState::render()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImVec2 windowSize = ImVec2(300, 400);
    ImGui::SetNextWindowPos(ImVec2((displaySize.x - windowSize.x) * 0.5f, (displaySize.y - windowSize.y) * 0.5f));
    ImGui::SetNextWindowSize(windowSize);

    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

    // Title / Logo
    ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize(LABEL("MENU_NAME")).x) * 0.5f);
    ImGui::Text(LABEL("MENU_NAME"));
    ImGui::Spacing();
    ImGui::Spacing();

    // Menu Buttons (Full width)
    ImVec2 buttonSize = ImVec2(-1, 40);
    if (ImGui::Button(LABEL("MENU_SINGLEPLAYER"), buttonSize)) { /* Start game */ }
    if (ImGui::Button(LABEL("MENU_MULTIPLAYER"), buttonSize)) { /* Start game */ }
    if (ImGui::Button(LABEL("MENU_SETTINGS"), buttonSize)) { /* Open settings */ }
    if (ImGui::Button(LABEL("MENU_CREDITS"), buttonSize)) { /* Open settings */ }
    if (ImGui::Button(LABEL("MENU_QUIT"), buttonSize)) {App::get()->quit();}

    ImGui::End();

    // Rendering
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MainMenuState::onExit()
{

}
