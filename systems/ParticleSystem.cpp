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
#include "ParticleData.h"
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
    GLuint shaderProgram { 0 };

    GLuint particlesBuffer { 0 };
    GLuint particlesColrosBuffer { 0 };

    GLuint VBO {0}, VAO {0}, EBO {0};

    glm::ivec3 drawPos;
    glm::ivec3 lookingAtParticlePos;
    float drawDistance { 1.0f };
    unsigned drawType = 2;

    // Settings
    int maxRaySteps { 400 };
    float particleScale { 0.4f };
    bool enableOutlines { false };
    unsigned int chunkSize { 50 }; // sorta unused now
    float stepDelay = 0.1f;

    ParticleData::Manager particleDataManager {glm::uvec3(chunkSize)};
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

    // for (unsigned x = 0; x < chunkSize; ++x) {
    //     for (unsigned y = 0; y < chunkSize; ++y) {
    //         for (unsigned z = 0; z < chunkSize; ++z) {
    //             particles.push_back(0);
    //         }
    //     }
    // }
    glNamedBufferStorage(
        particlesBuffer,
        sizeof(int) * particleDataManager.GetCubicSize(),
        (const void*)particleDataManager.GetParticleTypesData().data(),
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

    for (unsigned x = 10; x < 20; ++x) {
        for (unsigned y = 5; y < 15; ++y) {
            for (unsigned z = 10; z < 20; ++z) {
                // particles[z * (chunkSize * chunkSize) + y * (chunkSize) + x] = 2;
                particleDataManager.SetType(glm::uvec3(x, y, z), 2);
            }
        }
    }
}

void ParticleSystem::Update(float dt) {
    auto window = Graphics::GetWindow();

    if (dt == 0.0f) return;

    if (particleDataManager.Get(drawPos).particleType <= 1) {
        particleDataManager.SetType(drawPos, 0);
    }

    if (InputSystem::IsMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
        particleDataManager.SetType(drawPos, drawType);
    }

    if (InputSystem::IsMouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT)) {
        particleDataManager.SetType(drawPos, 0);
    }

    std::pair<double, double> mouseScroll = InputSystem::MouseScroll();

    drawDistance += static_cast<float>(mouseScroll.second);
    drawDistance = std::clamp(drawDistance, 0.0f, 100.0f);

    const glm::vec3 camPos = CameraSystem::GetPosition();
    const glm::vec3 camTarget = CameraSystem::GetTarget();

    const glm::vec3 lookingAt = camPos + (camTarget * drawDistance);

    // todo: it might be good to store the converted particle coordinate camPos and camTarget positions
    lookingAtParticlePos = lookingAt / particleScale;

    drawPos = glm::clamp(
        glm::ivec3(lookingAtParticlePos),
        glm::ivec3(0),
        glm::ivec3(particleDataManager.GetDimensions()) - 1
    );

    if (particleDataManager.Get(drawPos).particleType == 0) {
        particleDataManager.SetType(drawPos, 1);
    }

    static float step = 0.0f;
    if (step < stepDelay) {
        step += dt;
    }
    if (step >= stepDelay) {
        const glm::uvec3& dimensions = particleDataManager.GetDimensions();
        for (unsigned x = 0; x < dimensions.x; ++x) {
            for (int y = 0; y < dimensions.y; ++y) {
                for (unsigned z = 0; z < dimensions.z; ++z) {
                    const auto currentPos = glm::ivec3(x, y, z);

                    // z * (ysize * xsize) + y * (xsize) + x
                    unsigned particle = particleDataManager.Get(currentPos).particleType;
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

                        unsigned atNewPos = particleDataManager.Get(posToTry).particleType;
                        if (atNewPos != 0) continue;

                        // We are here, we are allowed to move here

                        particleDataManager.SetType(posToTry, particle);
                        // remove
                        particleDataManager.SetType(glm::uvec3(x, y, z), 0);
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
    glNamedBufferSubData(particlesBuffer, 0, sizeof(int) * particleDataManager.GetCubicSize(), (const void*)particleDataManager.GetParticleTypesData().data());
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
    // Fixme, chunksize xyz is no longer garenteed to be the same
    glUniform1ui(glGetUniformLocation(shaderProgram, "ChunkSize"), particleDataManager.GetDimensions().x);

    // Bind the vertex data
    glBindVertexArray(VAO);
    // Call draw
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

    if (ImGui::Begin("Simulation Controls")) {
        ImGui::SliderFloat("Simulation Step Speed", &stepDelay, 0.0f, 1.0f);
        ImGui::SliderInt("Max Ray Steps", &maxRaySteps, 0, 1000);
        ImGui::SliderFloat("Particle Scale", &particleScale, 0.001f, 5.0f, "%.8f");
        ImGui::Checkbox("Show Particle Outlines", &enableOutlines);
        ImGui::Text("Chunk Size (Currently Disabled) %i", chunkSize);
        // if (ImGui::SliderInt("Chunk Size", reinterpret_cast<int *>(&chunkSize), 10, 300)) {
        //     if (chunkSize > 0) {
        //         // Todo: add ability to resize?
        //         // particles.resize(chunkSize * chunkSize * chunkSize);
        //     }
        // }
    }
    ImGui::End();

    if (ImGui::Begin("Drawing Controls"))
    {
        ImGui::Text("Drawing is based on where you are looking");
        ImGui::Text("Hold Left Mouse Button to draw");
        ImGui::Text("Hold Right Mouse Button to erase");

        ImGui::Text("Drawing Position (In Chunk) %i %i %i", drawPos.x, drawPos.y, drawPos.z);
        ImGui::Text("Drawing Position (In World) %i %i %i", lookingAtParticlePos.x, lookingAtParticlePos.y, lookingAtParticlePos.z);
        ImGui::SliderInt("Draw Type", reinterpret_cast<int *>(&drawType), 0, ParticleTypeSystem::GetParticleTypeCount(), ParticleTypeSystem::GetParticleTypeInfo(drawType - 1).nameId);
    }
    ImGui::End();
}

void ParticleSystem::Exit() {}
void ParticleSystem::Done() {}

/* Public Function Implementation */


/* Private Function Implementation */
