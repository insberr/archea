//
// Created by insberr on 5/23/24.
//

#include "ImGuiSystem.h"
#include "../App.h"

ImGuiSystem::~ImGuiSystem() {
}

void ImGuiSystem::Init() {
    auto window = app->GetWindow();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void ImGuiSystem::Update(float dt) {

}

void ImGuiSystem::Render() {

}

void ImGuiSystem::Exit() {
    // Shutdown Imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiSystem::StartFrame() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiSystem::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiSystem::EnableImGui() {
    imGuiEnabled = true;

    // Allow ImGui to use the mouse
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void ImGuiSystem::DisableImGui() {
    imGuiEnabled = false;

    // Don't allow ImGui to use the mouse
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void ImGuiSystem::IsImGuiEnabled() {

}
