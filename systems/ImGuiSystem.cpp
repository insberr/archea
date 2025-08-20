//
// Created by insberr on 5/23/24.
//

#include "ImGuiSystem.h"
#include "../App.h"
#include "GraphicsSystem.h"

namespace ImGuiSystem {
    /* System Function Declarations */
    int Setup();

    void PreRender();
    void PostRender();
    void Done();
    System AsSystem() {
        return {
                .Setup = Setup,
                .PreRender = PreRender,
                .PostRender = PostRender,
                .Done = Done
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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(Graphics::GetWindow(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    return 0;
}

void ImGuiSystem::PreRender() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImGuiSystem::PostRender() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImGuiSystem::Done() {
    // Shutdown Imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiSystem::EnableImGui() {
    imGuiEnabled = true;

    // Allow ImGui to use the mouse and keyboard
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoKeyboard;
}

void ImGuiSystem::DisableImGui() {
    imGuiEnabled = false;

    // Don't allow ImGui to use the mouse and keybaord
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoKeyboard;
}

bool ImGuiSystem::IsImGuiEnabled() {
    return imGuiEnabled;
}
