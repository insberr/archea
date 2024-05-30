//
// Created by insberr on 5/21/24.
//

#include "App.h"

// Standard
#include <iostream>

// Systems
#include "systems/GraphicsSystem.h"
#include "systems/ImGuiSystem.h"


App::~App() {
    // Make sure we call in reverse order
    std::reverse(systems.begin(), systems.end());
    for (auto& system : systems) {
        system.Done();
    }
};

int App::Run() {
    // Initialize all systems
    for (auto& system : systems) {
        system.Init();
    }

    float dt = 0.0f;
    while (!glfwWindowShouldClose(Graphics::GetWindow())) {
        dt = timer.Mark();

        // Poll for events
        glfwPollEvents();

        for (auto& system : systems) {
            system.Update(dt);
        }

        // CLear the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start Imgui Frame
        ImGuiSystem::StartFrame();

        ImGui::ShowDemoWindow(); // Show demo window! :)
        if (ImGui::Begin("Stats")) {
            ImGui::Text("FPS %.2f", ImGui::GetIO().Framerate);
            ImGui::Text("Delta %.4f", dt);
        }
        ImGui::End();


        // Do drawing here
        for (auto& system : systems) {
            system.Render();
        }

        // Rendering Imgui
        ImGuiSystem::EndFrame();

        // Swap the buffers
        glfwSwapBuffers(Graphics::GetWindow());
    }

    // Call exit in reverse order
    std::reverse(systems.begin(), systems.end());
    for (auto& system : systems) {
        system.Exit();
    }

    return 0;
}

int App::AddSystem(const System& system)  {
    int setupCode = system.Setup();
    systems.push_back(system);
    return setupCode;
}
