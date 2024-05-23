//
// Created by insberr on 5/21/24.
//

#include <algorithm>

#include "ParticleSystem.h"
#include "../App.h"
#include "../shaders.h"
#include "InputSystem.h"

void ParticleSystem::Init() {
    // Load the contents of the shaders
    std::string vertexShaderSource = readShaderFile("shaders/vertex.glsl");
    std::string fragmentShaderSource = readShaderFile("shaders/fragment.glsl");

    // Make sure they arent empty
    if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
        return;
    }

    // Create the shader program
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0) return;

    // Set up vertex data and buffers and configure vertex attributes
    float quadVertices[] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            1.0f,  1.0f,
            -1.0f,  1.0f
    };

    GLuint indices[] = {
            0, 1, 2,
            2, 3, 0
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ParticleSystem::Update(float dt) {
    auto window = app->GetWindow();

    if (dt == 0.0f) return;

    if (InputSystem::IsKeyTriggered(GLFW_KEY_ESCAPE)) {
        cursorLocked = !cursorLocked;
        if (cursorLocked) {
            // Lock the cursor and hide it
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            // Lock the cursor and hide it
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    // auto input = app->GetSystem<InputSystem>();

    if (InputSystem::IsKeyHeld(GLFW_KEY_W)) {
        posZ += dt;
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_S)) {
        posZ -= dt;
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_A)) {
        posX -= dt;
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_D)) {
        posX += dt;
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_SPACE)) {
        posY += dt;
    }if (InputSystem::IsKeyHeld(GLFW_KEY_LEFT_SHIFT)) {
        posY -= dt;
    }
    
}

void ParticleSystem::Render() {
    auto window = app->GetWindow();

    // Set the shader program
    glUseProgram(shaderProgram);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glUniform2f(glGetUniformLocation(shaderProgram, "Resolution"), width, height);
    glUniform1f(glGetUniformLocation(shaderProgram, "Time"), glfwGetTime());

    // Get the mouse position
    auto [xPos, yPos] = InputSystem::MousePosition();
    glUniform2f(glGetUniformLocation(shaderProgram, "Mouse"), static_cast<float>(xPos), static_cast<float>(yPos));

    glUniform3f(glGetUniformLocation(shaderProgram, "CameraPosition"), posX, posY, posZ);

    // Bind the vertex data
    glBindVertexArray(VAO);
    // Call draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ParticleSystem::Exit() {

}
