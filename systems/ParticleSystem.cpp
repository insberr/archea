//
// Created by insberr on 5/21/24.
//

#include <algorithm>

#include "ParticleSystem.h"

#include "../shaders.h"

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
    if (dt == 0.0f) return;
}

void ParticleSystem::Render(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_W)) {
        posZ += 0.2f;
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
        posZ -= 0.2f;
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        posX -= 0.2f;
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        posX += 0.2f;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        posY += 0.2f;
    }if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
        posY -= 0.2f;
    }



    // Set the shader program
    glUseProgram(shaderProgram);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glUniform2f(glGetUniformLocation(shaderProgram, "Resolution"), width, height);
    glUniform1f(glGetUniformLocation(shaderProgram, "Time"), glfwGetTime());

    // Get the mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
//    xpos = std::ranges::clamp(xpos, 0.0, static_cast<double>(width));
//    ypos = std::ranges::clamp(ypos, 0.0, static_cast<double>(height));
    glUniform2f(glGetUniformLocation(shaderProgram, "Mouse"), static_cast<float>(xpos), static_cast<float>(ypos));

    glUniform3f(glGetUniformLocation(shaderProgram, "CameraPosition"), posX, posY, posZ);

    // Bind the vertex data
    glBindVertexArray(VAO);
    // Call draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ParticleSystem::Exit() {

}
