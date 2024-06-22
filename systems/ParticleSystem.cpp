//
// Created by insberr on 5/21/24.
//

#include "ParticleSystem.h"

#include <algorithm>
#include <vector>

#include "GraphicsSystem.h"
#include "../shaders.h"
// glm
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "InputSystem.h"
#include "CameraSystem.h"
#include "ImGuiSystem.h"
#include "particle_types/ParticleTypeSystem.h"
#include "particle_types/ParticleType.h"

namespace ParticleSystem {
    /* System Function Declarations */
    int Setup();
    void Init();
    void Update(float dt);
    void Render();
    void Exit();
    void Done();
    System AsSystem() {
        return {
            Setup,
            Init,
            Update,
            Render,
            Exit,
            Done
        };
    }

    /* Private Variables And Functions */

    std::vector<int> particles;
    GLuint shaderProgram { 0 };

    GLuint particlesBuffer { 0 };
    GLuint particlesColrosBuffer { 0 };

    GLuint VBO {0}, VAO {0}, EBO {0};

    glm::ivec3 drawPos;

    // Settings
    int maxRaySteps { 200 };
    float particleScale { 0.4f };
    bool enableOutlines { false };
};

int ParticleSystem::Setup() { return 0; }
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
    // Define the vertices for a cube
    float cubeVertices[] = {
            // Front face
            -0.5f, -0.5f,  0.5f,  // 0
            0.5f, -0.5f,  0.5f,  // 1
            0.5f,  0.5f,  0.5f,  // 2
            -0.5f,  0.5f,  0.5f,  // 3

            // Back face
            -0.5f, -0.5f, -0.5f,  // 4
            0.5f, -0.5f, -0.5f,  // 5
            0.5f,  0.5f, -0.5f,  // 6
            -0.5f,  0.5f, -0.5f,  // 7

            // Top face
            0.5f,  0.5f,  0.5f,  // 8
            -0.5f,  0.5f,  0.5f,  // 9
            -0.5f,  0.5f, -0.5f,  // 10
            0.5f,  0.5f, -0.5f,  // 11

            // Bottom face
            -0.5f, -0.5f,  0.5f,  // 12
            0.5f, -0.5f,  0.5f,  // 13
            0.5f, -0.5f, -0.5f,  // 14
            -0.5f, -0.5f, -0.5f,  // 15

            // Right face
            0.5f, -0.5f,  0.5f,  // 16
            0.5f, -0.5f, -0.5f,  // 17
            0.5f,  0.5f, -0.5f,  // 18
            0.5f,  0.5f,  0.5f,  // 19

            // Left face
            -0.5f, -0.5f,  0.5f,  // 20
            -0.5f, -0.5f, -0.5f,  // 21
            -0.5f,  0.5f, -0.5f,  // 22
            -0.5f,  0.5f,  0.5f   // 23
    };

// Define the indices for the cube
    GLuint indices[] = {
            // Front face
            0, 1, 2,
            2, 3, 0,

            // Back face
            4, 5, 6,
            6, 7, 4,

            // Top face
            8, 9, 10,
            10, 11, 8,

            // Bottom face
            12, 13, 14,
            14, 15, 12,

            // Right face
            16, 17, 18,
            18, 19, 16,

            // Left face
            20, 21, 22,
            22, 23, 20
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glCreateBuffers(1, &particlesBuffer);

    for (unsigned x = 0; x < 50; ++x) {
        for (unsigned y = 0; y < 50; ++y) {
            for (unsigned z = 0; z < 50; ++z) {
                particles.push_back(0);
            }
        }
    }
    glNamedBufferStorage(
        particlesBuffer,
        sizeof(int) * (50 * 50 * 50),
        (const void*)particles.data(),
        GL_DYNAMIC_STORAGE_BIT
    );

    glCreateBuffers(1, &particlesColrosBuffer);
    // std::vector<glm::vec4> particleColors;
    // add colors
    // particleColors.push_back(glm::vec4(255, 0, 0, 50) / 255.0f); // debug/draw
    // particleColors.push_back(glm::vec4(200, 150, 10, 255) / 255.0f); // sand
    // particleColors.push_back(glm::vec4(13, 136, 188, 100) / 255.0f); // water
    // particleColors.push_back(glm::vec4(239, 103, 23, 255) / 255.0f); // lava
    auto particleColors = ParticleTypeSystem::GetParticleColorIndexesForShader();
    glNamedBufferStorage(
        particlesColrosBuffer,
        sizeof(NormColor) * particleColors.size(),
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
    auto window = Graphics::GetWindow();

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
                    const auto currentPos = glm::ivec3(x, y, z);

                    // z * (ysize * xsize) + y * (xsize) + x
                    int particle = particles[z * (50 * 50) + y * (50) + x];
                    if (particle <= 1) continue;

                    auto particleTypeInfo = ParticleTypeSystem::GetParticleTypeInfo(particle - 1);

                    auto posToTry = glm::ivec3(0);

                    // new
                    auto nextMove = ParticleMove::MoveState {};
                    while (true) {
                        particleTypeInfo.getNextMove(nextMove);

                        if (nextMove.done) break;

                        posToTry = currentPos + glm::ivec3(nextMove.positionToTry);

                        // try pos

                        if (posToTry.y < 0) continue;
                        posToTry = glm::clamp(posToTry, glm::ivec3(0), glm::ivec3(49));
                        // int newY = std::clamp<int>(y - 1, 0, 49);

                        int atNewPos = particles[posToTry.z * (50 * 50) + posToTry.y * (50) + posToTry.x];
                        if (atNewPos != 0) continue;
                        particles[posToTry.z * (50 * 50) + posToTry.y * (50) + posToTry.x] = particle;
                        // remove
                        particles[z * (50 * 50) + y * (50) + x] = 0;
                        break;
                        // if works, break
                        // else continue
                    }
                    //end
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
    auto window = Graphics::GetWindow();

    // Bind the particles data
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particlesBuffer);
    // Bind the particles color data
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particlesColrosBuffer);

    // Set the shader program
    glUseProgram(shaderProgram);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    // glUniform2f(glGetUniformLocation(shaderProgram, "Resolution"), width, height);
    glUniform1f(glGetUniformLocation(shaderProgram, "Time"), glfwGetTime());

    auto cameraFieldOfView = CameraSystem::GetFOV();
    auto cameraPos = CameraSystem::GetPosition();
    auto cameraMatrix = CameraSystem::CameraMatrix();
    // glUniform1f(glGetUniformLocation(shaderProgram, "FieldOfView"), cameraFieldOfView);
    glUniform3f(glGetUniformLocation(shaderProgram, "CameraPosition"), cameraPos.x, cameraPos.y, cameraPos.z);
    // glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "CameraView"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));

    // Pass the matrices to the shader (for vertex shader for now)
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    auto model = CameraSystem::GetModel();
    auto view = CameraSystem::GetView();
    auto projection = CameraSystem::GetProjection();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform1i(glGetUniformLocation(shaderProgram, "MAX_RAY_STEPS"), maxRaySteps);
    glUniform1f(glGetUniformLocation(shaderProgram, "ParticleScale"), particleScale);
    glUniform1ui(glGetUniformLocation(shaderProgram, "EnableOutlines"), enableOutlines);

    // Bind the vertex data
    glBindVertexArray(VAO);
    // Call draw
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

    if (ImGui::Begin("Simulation Controls")) {
        ImGui::Text("Draw Pos (Change With Arrow Keys) %i %i %i", drawPos.x, drawPos.y, drawPos.z);
        ImGui::SliderInt("Max Ray Steps", &maxRaySteps, 0, 1000);
        ImGui::SliderFloat("Particle Scale", &particleScale, 0.001f, 5.0f, "%.8f");
        ImGui::Checkbox("Show Particle Outlines", &enableOutlines);
    }
    ImGui::End();
}

void ParticleSystem::Exit() {}
void ParticleSystem::Done() {}

/* Public Function Implementation */


/* Private Function Implementation */
