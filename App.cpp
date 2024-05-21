//
// Created by insberr on 5/21/24.
//

#include "App.h"

#include <iostream>

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

App::App() {
    int code = Init();
    if (code) {
        errorCode = code;
    }
}

App::~App() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

int App::Run() {
    if (errorCode) return errorCode;

    for (auto & system : systems) {
        system->Init();
    }

    while (!glfwWindowShouldClose(window)) {
        for (auto & system : systems) {
            system->Update(0.0f);
        }

        // CLear the window
        glClear(GL_COLOR_BUFFER_BIT);

        // Do drawing here
        for (auto & system : systems) {
            system->Render();
        }

        // Swap the buffers
        glfwSwapBuffers(window);

        // Poll for events
        glfwPollEvents();
    }

    for (auto & system : systems) {
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

void App::AddSystem(System *system) {
    systems.push_back(system);
}
