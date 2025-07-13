//
// Created by insberr on 5/21/24.
//

#include "App.h"

// Standard
#include <iostream>
// #include <chrono>
// #include <thread>

// Systems
#include "scenes/MainMenuScene.h"
#include "systems/GraphicsSystem.h"
#include "systems/ImGuiSystem.h"
#include "systems/SceneSystem.h"

static bool do_shutdown = false;

App::~App() {
    // Make sure we call in reverse order
    // Already in reverse order from Exit call
    // std::reverse(systems.begin(), systems.end());
    for (const System &system: systems) {
        if (system.Done) system.Done();
    }

    do_shutdown = false;
};

int App::Run() {
    // Initialize all systems
    for (const System &system: systems) {
        if (system.Init) system.Init();
    }

    SceneSystem::AddScene(new MainMenuScene());
    SceneSystem::SwitchActiveScene("MainMenuScene");

    float dt = 0.0f;
    while (!glfwWindowShouldClose(Graphics::GetWindow()) && !do_shutdown) {
        dt = timer.Mark();

        // todo: frame cap
        // float maxFrameMSForFPS = 1000.0f / 120.0f;
        // float usedMSLastFrame = dt * 1000.0f;
        // int waitMS = static_cast<int>(maxFrameMSForFPS - usedMSLastFrame);
        // std::chrono::milliseconds timespan(waitMS);
        // std::this_thread::sleep_for(timespan);

        for (const System &system: systems) {
            if (system.PrePollEvents) system.PrePollEvents();
        }

        // Poll for events
        glfwPollEvents();

        for (const System &system: systems) {
            if (system.Update) system.Update(dt);
        }

        // CLear the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const System &system: systems) {
            if (system.PreRender) system.PreRender();
        }

        // Do drawing here
        for (const System &system: systems) {
            if (system.Render) system.Render();
        }

        if (ImGui::Begin("Stats")) {
            ImGui::Text("FPS %.2f", ImGui::GetIO().Framerate);
            ImGui::Text("Delta %.4f", dt);
        }
        ImGui::End();

        for (const System &system: systems) {
            if (system.PostRender) system.PostRender();
        }

        // Swap the buffers
        glfwSwapBuffers(Graphics::GetWindow());
    }

    // Call exit in reverse order
    std::reverse(systems.begin(), systems.end());
    for (auto &system: systems) {
        if (system.Exit) system.Exit();
    }

    return 0;
}

int App::AddSystem(const System &system) {
    int setupCode = system.Setup ? system.Setup() : 0;
    systems.push_back(system);
    return setupCode;
}

void App::Shutdown() {
    do_shutdown = true;
}
