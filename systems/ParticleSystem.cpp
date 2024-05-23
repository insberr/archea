//
// Created by insberr on 5/21/24.
//

#include <algorithm>

#include "ParticleSystem.h"
#include "../App.h"
#include "../shaders.h"
#include "InputSystem.h"
#include "CameraSystem.h"
#include "ImGuiSystem.h"

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

    glCreateBuffers(1, &particlesBuffer);
    particles.reserve(50 * 50 * 50);
    for (auto& part : particles) {
        part = 0;
    }
    glNamedBufferStorage(
        particlesBuffer,
        sizeof(int) * (50 * 50 * 50),
        (const void*)particles.data(),
        GL_DYNAMIC_STORAGE_BIT
    );

    glCreateBuffers(1, &particlesColrosBuffer);
    std::vector<glm::vec4> particleColors;
    // add colors
    particleColors.push_back(glm::vec4(200, 150, 10, 255) / 255.0f);
    particleColors.push_back(glm::vec4(13, 136, 188, 100) / 255.0f);
    particleColors.push_back(glm::vec4(239, 103, 23, 255) / 255.0f);
    glNamedBufferStorage(
        particlesColrosBuffer,
        sizeof(glm::vec4) * particleColors.size(),
        (const void*)particleColors.data(),
        GL_DYNAMIC_STORAGE_BIT
    );
}

void ParticleSystem::Update(float dt) {
    auto window = app->GetWindow();

    if (dt == 0.0f) return;

    if (InputSystem::IsKeyHeld(GLFW_KEY_Q)) {
        particles.push_back(3);
    }
    glNamedBufferSubData(particlesBuffer, 0, sizeof(int) * (50 * 50 * 50), (const void*)particles.data());
}

void ParticleSystem::Render() {
    auto window = app->GetWindow();

    // Bind the particles data
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particlesBuffer);
    // Bind the particles color data
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particlesColrosBuffer);

    // Set the shader program
    glUseProgram(shaderProgram);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glUniform2f(glGetUniformLocation(shaderProgram, "Resolution"), width, height);
    glUniform1f(glGetUniformLocation(shaderProgram, "Time"), glfwGetTime());

    auto camera = app->GetSystem<CameraSystem>();
    auto cameraFieldOfView = camera->GetFOV();
    auto cameraPos = camera->GetPosition();
    auto cameraMatrix = camera->CameraMatrix();
    glUniform1f(glGetUniformLocation(shaderProgram, "FieldOfView"), cameraFieldOfView);
    glUniform3f(glGetUniformLocation(shaderProgram, "CameraPosition"), cameraPos.x, cameraPos.y, cameraPos.z);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "CameraView"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));

    glUniform1i(glGetUniformLocation(shaderProgram, "MAX_RAY_STEPS"), maxRaySteps);
    // Bind the vertex data
    glBindVertexArray(VAO);
    // Call draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

    if (ImGui::Begin("Simulation Controls")) {
        ImGui::SliderInt("Max Ray Steps", &maxRaySteps, 0, 1000);
    }
    ImGui::End();
}

void ParticleSystem::Exit() {

}
