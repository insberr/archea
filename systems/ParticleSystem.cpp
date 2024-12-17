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
#include <set>

#include "InputSystem.h"
#include "CameraSystem.h"
#include "ImGuiSystem.h"
#include "ParticleData.h"
#include "ParticlesChunk.h"
#include "Shapes.h"
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

    // GLuint particlesBuffer { 0 };
    GLuint particlesColrosBuffer { 0 };

    GLuint VBO {0}, VAO {0}, EBO {0};

    glm::ivec3 drawPos;
    glm::ivec3 lookingAtParticlePos;
    float drawDistance { 1.0f };
    unsigned drawType = 2;

    // Settings
    // int maxRaySteps { 400 };
    // bool enableOutlines { false };
    // init ParticlesChunk static values
    float particleScale { 0.4f };
    unsigned int chunkSize { 50 };
    float stepDelay = 0.1f;

    // Chunks
    std::vector<ParticlesChunk*> particleChunks;
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
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes::Cube::cubeVertices), Shapes::Cube::cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Shapes::Cube::indices), &Shapes::Cube::indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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

    ChunkConfig::enableOutlines = false;
    ChunkConfig::maxRaySteps = 400;

    for (unsigned x = 0; x < 2; ++x) {
        for (unsigned y = 0; y < 1; ++y) {
            for (unsigned z = 0; z < 2; ++z) {
                std::cout << "Init chunk at pos " << x << " " << y << " " << z << std::endl;
                particleChunks.push_back(
                    new ParticlesChunk(glm::ivec3(x, y, z), glm::uvec3(chunkSize), particleScale)
                );
            }
        }
    }
}

void ParticleSystem::Update(float dt) {
    if (dt == 0.0f) return;

    // if (particleDataManager.Get(drawPos).particleType <= 1) {
    //     particleDataManager.SetType(drawPos, 0);
    // }

    std::pair<double, double> mouseScroll = InputSystem::MouseScroll();

    drawDistance += static_cast<float>(mouseScroll.second);
    drawDistance = std::clamp(drawDistance, 0.0f, 100.0f);

    const glm::vec3 camPos = CameraSystem::GetPosition();
    const glm::vec3 camTarget = CameraSystem::GetTarget();

    const glm::vec3 lookingAt = camPos + (camTarget * drawDistance);

    // todo: it might be good to store the converted particle coordinate camPos and camTarget positions
    lookingAtParticlePos = lookingAt / particleScale;

    if (InputSystem::IsMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
        // particleDataManager.SetType(drawPos, drawType);
        // todo: this is not really efficient
        for (const auto& chunk : particleChunks) {
            chunk->TryPlaceParticleAt(
                lookingAtParticlePos,
                { glm::uvec3(0), drawType, { glm::vec3(0), 0.0f } }
            );
        }
    }

    if (InputSystem::IsMouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT)) {
        // particleDataManager.SetType(drawPos, 0);
        for (const auto& chunk : particleChunks) {
            chunk->TryPlaceParticleAt(
                lookingAtParticlePos,
                { glm::uvec3(0), 0, { glm::vec3(0), 0.0f } }
            );
        }
    }

    // drawPos = glm::clamp(
    //     glm::ivec3(lookingAtParticlePos),
    //     glm::ivec3(0),
    //     glm::ivec3(particleDataManager.GetDimensions()) - 1
    // );
    //
    // if (particleDataManager.Get(drawPos).particleType == 0) {
    //     particleDataManager.SetType(drawPos, 1);
    // }

    auto cameraChunkPosition = PositionConversion::WorldPositionToChunkPosition(CameraSystem::GetPosition() / particleScale, glm::uvec3(chunkSize));
    // std::cout << "Camera is in chunk " << cameraChunkPosition.x << " " << cameraChunkPosition.y << " " << cameraChunkPosition.z << std::endl;
    const int chunkDistance = 2; // Distance to load chunks around the player

    struct IVec3Comparator {
        bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
            if (a.x != b.x) return a.x < b.x;
            if (a.y != b.y) return a.y < b.y;
            return a.z < b.z;
        }
    };
    // Determine the required chunk positions around the camera
    std::set<glm::ivec3, IVec3Comparator> requiredChunkPositions;
    for (int x = -chunkDistance; x <= chunkDistance; ++x) {
        for (int z = -chunkDistance; z <= chunkDistance; ++z) {
            auto temp_cameraChunkPosition = cameraChunkPosition;
            // Enforce y to always be 0. We don't do vertical chunks right now
            temp_cameraChunkPosition.y = 0;
            requiredChunkPositions.insert(cameraChunkPosition + glm::ivec3(x, 0, z));
        }
    }

    // Manage chunks in one pass
    std::vector<ParticlesChunk*> updatedChunks;

    for (auto& chunk : particleChunks) {
        auto pos = chunk->getChunkWorldPosition();
        if (requiredChunkPositions.count(pos)) {
            // Keep chunk if it's within range
            updatedChunks.push_back(chunk);
            // Remove from required set
            requiredChunkPositions.erase(pos);
        } else {
            // Remove chunks outside range
            delete chunk;
        }
    }

    // Add missing chunks
    for (const auto& pos : requiredChunkPositions) {
        auto* newChunk = new ParticlesChunk(pos, glm::uvec3(chunkSize), particleScale);

        updatedChunks.push_back(newChunk);
    }

    particleChunks = std::move(updatedChunks);


    static float step = 0.0f;
    if (step < stepDelay) {
        step += dt;
    }
    if (step >= stepDelay) {
        // todo: multithreading stuff
        std::vector<std::jthread> threads;
        for (const auto& chunk : particleChunks) {
            threads.emplace_back([&chunk]() {
                chunk->ProcessNextSimulationStep();
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        step = 0.0f;
    }

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

    // todo: call render on all chunks
    for (const auto& chunk : particleChunks) {
        chunk->Render(window, shaderProgram, particlesColrosBuffer, VAO);
    }

    if (ImGui::Begin("Simulation Controls")) {
        ImGui::SliderFloat("Simulation Step Speed", &stepDelay, 0.0f, 1.0f);
        ImGui::SliderInt("Max Ray Steps", &ChunkConfig::maxRaySteps, 0, 1000);
        ImGui::SliderFloat("Particle Scale", &particleScale, 0.001f, 5.0f, "%.8f");
        ImGui::Checkbox("Show Particle Outlines", &ChunkConfig::enableOutlines);
        ImGui::Text("Chunk Size (Currently Disabled) %i", chunkSize);
        // if (ImGui::SliderInt("Chunk Size", reinterpret_cast<int *>(&chunkSize), 10, 300)) {
        //     if (chunkSize > 0) {
        //         // Todo: add ability to resize?
        //         // particles.resize(chunkSize * chunkSize * chunkSize);
        //     }
        // }
    }
    ImGui::End();

    if (ImGui::Begin("Drawing Controls (not working right now)"))
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

void ParticleSystem::Exit() {

}
void ParticleSystem::Done() {
    for (const auto& chunk : particleChunks) {
        delete chunk;
    }
}

/* Public Function Implementation */


/* Private Function Implementation */
