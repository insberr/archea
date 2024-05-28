//
// Created by insberr on 5/27/24.
//

#include "GraphicsSystem.h"

#include <iostream>

// Private values
namespace Graphics {
    InstanceClass self {};

    GLFWwindow* window { nullptr };

    /* Internal Functions */

    // Callback function for errors
    void errorCallback(int error, const char* description);

    // Some callback for frame buffer size
    void framebufferSizeCallback(GLFWwindow* window, int width, int height);
}

int Graphics::InstanceClass::Setup() {
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
    glClearColor(0.4f, 0.4f, 0.4f, 0.4f);

    return 0;
}

void Graphics::InstanceClass::Init() {}
void Graphics::InstanceClass::Exit() {}

void Graphics::InstanceClass::Done() {
    // Free GLFW window, GLFW, and GLEW if needed

    // Shutdown GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

Graphics::InstanceClass &Graphics::Instance() {
    return self;
}

System *Graphics::AsSystem() {
    return reinterpret_cast<System*>(&self);
}

GLFWwindow* Graphics::GetWindow() {
    return window;
}

GLuint Graphics::CreateShaderProgram() {
    return 0; // todo
}

/* Private Internal Functions */
void Graphics::errorCallback(int error, const char *description) {
    std::cerr << "Error: " << description << std::endl;
}

void Graphics::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
