//
// Created by insberr on 5/27/24.
//

#include "GraphicsSystem.h"

#include <iostream>

#include "CameraSystem.h"
#include "Shapes.h"
#include "../shaders.h"

// Private values
namespace Graphics {
    /* System Function Declarations */
    int Setup();
    void Init();
    void Exit();
    void Done();
    System AsSystem() {
        return {
                .Setup = Setup,
                .Init = Init,
                .Exit = Exit,
                .Done = Done,
        };
    }

    /* Private Variables And Functions */

    // The GLFW window
    GLFWwindow* window { nullptr };

    /* Internal Functions */

    // Callback function for errors
    void errorCallback(int error, const char* description);

    // Some callback for frame buffer size
    void framebufferSizeCallback(GLFWwindow* window, int width, int height);
}

namespace Graphics::Draw2D {
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;

    void setupRectBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes::Rect::rectVertices), Shapes::Rect::rectVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Shapes::Rect::rectIndices), Shapes::Rect::rectIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void DrawRectangle(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color) {
        glUseProgram(shaderProgram);

        // Set uniforms
        glUniform2f(glGetUniformLocation(shaderProgram, "u_position"), position.x, position.y);
        glUniform2f(glGetUniformLocation(shaderProgram, "u_size"), size.x, size.y);
        // TODO: Dont hard code this...
        glUniform2f(glGetUniformLocation(shaderProgram, "u_resolution"), 1920, 1080);
        glUniform3f(glGetUniformLocation(shaderProgram, "u_color"), color.r, color.g, color.b);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

int Graphics::Setup() {
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

    // Request to disable vsync
    // glfwSwapInterval(0);

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

    // Enable blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.4f, 0.4f, 0.4f, 1.0);

    return 0;
}

void Graphics::Init() {
    Draw2D::setupRectBuffers();
    // Load the contents of the shaders
    std::string vertexShaderSource = readShaderFile("shaders/vertex_rect.glsl");
    std::string fragmentShaderSource = readShaderFile("shaders/fragment_rect.glsl");

    // Make sure they arent empty
    if (!vertexShaderSource.empty() && !fragmentShaderSource.empty()) {
        // Create the shader program
        Draw2D::shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    }
}
void Graphics::Exit() {}

void Graphics::Done() {
    // Free GLFW window, GLFW, and GLEW if needed

    // Shutdown GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
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

void Graphics::framebufferSizeCallback(GLFWwindow* wind, int width, int height) {
    glViewport(0, 0, width, height);
    CameraSystem::setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
}
