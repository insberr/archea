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
    particleColors.push_back(glm::vec4(255, 0, 0, 50) / 255.0f);
    particleColors.push_back(glm::vec4(200, 150, 10, 255) / 255.0f);
    particleColors.push_back(glm::vec4(13, 136, 188, 100) / 255.0f);
    particleColors.push_back(glm::vec4(239, 103, 23, 255) / 255.0f);
    glNamedBufferStorage(
        particlesColrosBuffer,
        sizeof(glm::vec4) * particleColors.size(),
        (const void*)particleColors.data(),
        GL_DYNAMIC_STORAGE_BIT
    );

    for (unsigned x = 2; x < 7; ++x) {
        for (unsigned y = 5; y < 10; ++y) {
            for (unsigned z = 2; z < 7; ++z) {
                particles[z * (50 * 50) + y * (50) + x] = 2;
            }
        }
    }
}

void ParticleSystem::Update(float dt) {
    auto window = app->GetWindow();

    if (dt == 0.0f) return;

    int& part = particles[drawPos.z * (50 * 50) + drawPos.y * (50) + drawPos.x];
    if (part <= 1) {
        part = 0;
    }

    if (InputSystem::IsKeyHeld(GLFW_KEY_Q)) {
        particles[drawPos.z * (50 * 50) + drawPos.y * (50) + drawPos.x] = 2;
    }

    if (InputSystem::IsKeyHeld(GLFW_KEY_LEFT)) {
        drawPos.x = std::clamp(drawPos.x - 1, 0, 50 - 1);
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_RIGHT)) {
        drawPos.x = std::clamp(drawPos.x + 1, 0, 50 - 1);
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_F)) {
        if (InputSystem::IsKeyHeld(GLFW_KEY_UP)) {
            drawPos.z = std::clamp(drawPos.z + 1, 0, 50 - 1);
        }
        if (InputSystem::IsKeyHeld(GLFW_KEY_DOWN)) {
            drawPos.z = std::clamp(drawPos.z - 1, 0, 50 - 1);
        }
    } else {
        if (InputSystem::IsKeyHeld(GLFW_KEY_UP)) {
            drawPos.y = std::clamp(drawPos.y + 1, 0, 50 - 1);
        }
        if (InputSystem::IsKeyHeld(GLFW_KEY_DOWN)) {
            drawPos.y = std::clamp(drawPos.y - 1, 0, 50 - 1);
        }
    }
    int& part2 = particles[drawPos.z * (50 * 50) + drawPos.y * (50) + drawPos.x];
    if (part2 <= 1) {
        part2 = 1;
    }

    static float step = 0.0f;
    if (step < 0.4f) {
        step += dt;
    }
    if (step >= 0.4f) {
        for (unsigned x = 0; x < 50; ++x) {
            for (int y = 0; y < 50; ++y) {
                for (unsigned z = 0; z < 50; ++z) {
                    // z * (ysize * xsize) + y * (xsize) + x
                    int particle = particles[z * (50 * 50) + y * (50) + x];
                    if (particle <= 1) continue;

                    int newY = std::clamp<int>(y - 1, 0, 49);
                    int atNewY = particles[z * (50 * 50) + newY * (50) + x];
                    if (atNewY != 0) continue;
                    particles[z * (50 * 50) + newY * (50) + x] = particle;
                    // remove
                    particles[z * (50 * 50) + y * (50) + x] = 0;
                }
            }
        }
        step = 0.0f;
    }
    glNamedBufferSubData(particlesBuffer, 0, sizeof(int) * (50 * 50 * 50), (const void*)particles.data());
}

//struct Particel {
//    Type t;
//    glm::vec4 color;
//    float temperature;
//};
//class Particles {
//    Particel GetParticle(glm::vec3 pos) {
//        return {
//            particles[pos.z * (50 * 50) + pos.y * (50) + pos.x],
//            color,
//            10.0f
//        };
//    }
//private:
//
//    std::vector<glm::vec4> particles;
//
//};

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

    // Pass the matrices to the shader (for vertex shader for now)
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    auto model = camera->GetModel();
    auto view = camera->GetView();
    auto projection = camera->GetProjection();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform1i(glGetUniformLocation(shaderProgram, "MAX_RAY_STEPS"), maxRaySteps);
    glUniform1f(glGetUniformLocation(shaderProgram, "ParticleScale"), particleScale);
    glUniform1ui(glGetUniformLocation(shaderProgram, "EnableOutlines"), enableOutlines);

    // Bind the vertex data
    glBindVertexArray(VAO);
    // Call draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

    if (ImGui::Begin("Simulation Controls")) {
        ImGui::Text("Draw Pos (Change With Arrow Keys) %i %i %i", drawPos.x, drawPos.y, drawPos.z);
        ImGui::SliderInt("Max Ray Steps", &maxRaySteps, 0, 1000);
        ImGui::SliderFloat("Particle Scale", &particleScale, 0.001f, 2.0f, "%.3f");
        ImGui::Checkbox("Show Particle Outlines", &enableOutlines);
    }
    ImGui::End();
}

void ParticleSystem::Exit() {

}
