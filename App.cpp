//
// Created by insberr on 5/21/24.
//

#include "App.h"

#include <iostream>
#include "systems/CameraSystem.h"
#include "systems/ImGuiSystem.h"

// Some constants
const unsigned WindowWidth = 1280;
const unsigned WindowHeight = 720;

// Callback function for errors
void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

App::App() : window(nullptr) {
    int code = Init();
    if (code) {
        errorCode = code;
    }
}

App::~App() {
    // Shutdown GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

int App::Run() {
    if (errorCode) return errorCode;

    for (auto & [systemTypeId, system] : systems) {
        system->Init();
    }

    float dt = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        dt = timer.Mark();

        // Poll for events
        glfwPollEvents();

        for (auto & [systemTypeId, system] : systems) {
            system->Update(dt);
        }

        // CLear the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start Imgui Frame
        GetSystem<ImGuiSystem>()->StartFrame();

        ImGui::ShowDemoWindow(); // Show demo window! :)
        if (ImGui::Begin("Stats")) {
            ImGui::Text("FPS %.2f", ImGui::GetIO().Framerate);
            ImGui::Text("Delta %.4f", dt);
        }
        ImGui::End();

        // Do drawing here
        for (auto & [systemTypeId, system] : systems) {
            system->Render();
        }

        // Rendering Imgui
        GetSystem<ImGuiSystem>()->EndFrame();

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    for (auto & [systemTypeId, system] : systems) {
        system->Exit();
    }

    return 0;
}

int App::Init() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set the GLFW error callback
    glfwSetErrorCallback(errorCallback);

    // Give hints to glfw about version we wish to use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window using glfw
    window = glfwCreateWindow(WindowWidth, WindowHeight, "Archea", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make window context current
    glfwMakeContextCurrent(window);
    // note: figure out what this does too
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    return 0;
}

//void App::AddSystem(System* system) {
//    systems.push_back(system);
//}

GLFWwindow* App::GetWindow() {
    return window;
}

//System* App::GetSystem(std::string &systemName) {
//    return systems[0];
//}
