//
// Created by insberr on 5/21/24.
//

#include "App.h"

#include <iostream>
#include "systems/System.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Some constants
const unsigned WindowWidth = 800;
const unsigned WindowHeight = 600;

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

App::~App() {
    // Shutdown Imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Shutdown GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

int App::Run() {
    if (errorCode) return errorCode;

    for (auto & [systemTypeId, system] : systems) {
        system->Init();
    }

    while (!glfwWindowShouldClose(window)) {
        // Poll for events
        glfwPollEvents();

        for (auto & [systemTypeId, system] : systems) {
            system->Update(0.0f);
        }

        // CLear the window
        glClear(GL_COLOR_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow(); // Show demo window! :)
        if (ImGui::Begin("Stats")) {
            ImGui::Text("FPS %.2f", ImGui::GetIO().Framerate);
        }
        ImGui::End();
        // Do drawing here
        for (auto & [systemTypeId, system] : systems) {
            system->Render();
        }

        // Rendering Imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
