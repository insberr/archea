//
// Created by insberr on 5/23/24.
//

#include "ImGuiSystem.h"
#include "../App.h"
#include "GraphicsSystem.h"

namespace ImGuiSystem {
    /* System Function Declarations */
    int Setup();
    // void Init();
    void Update(float dt);
    void Render();
    // void Exit();
    void Done();
    System AsSystem() {
        return {
                Setup,
                nullptr, // Init,
                Update,
                Render,
                nullptr, // Exit,
                Done
        };
    }

    /* Private Variables And Functions */

    bool imGuiEnabled { true };
}

int ImGuiSystem::Setup() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(Graphics::GetWindow(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    return 0;
}

void ImGuiSystem::Update(float dt) {

}

void ImGuiSystem::Render() {

}

void ImGuiSystem::Done() {
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

bool ImGuiSystem::IsImGuiEnabled() {
    return imGuiEnabled;
}
