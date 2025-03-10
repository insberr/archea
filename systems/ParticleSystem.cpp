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
#include <thread>

#include "InputSystem.h"
#include "CameraSystem.h"
#include "ImGuiSystem.h"
#include "ParticleData.h"
#include "ParticlesChunk.h"
#include "Player.h"
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

    // Settings
    // int maxRaySteps { 400 };
    // bool enableOutlines { false };
    // init ParticlesChunk static values
    int simulationStepInterval { 200 };
    float simulationStepDelta { 1.0f };

    // Chunks
    std::vector<ParticlesChunk*> particleChunks;
    std::mutex chunksLock;
    std::jthread chunksThread;

    Player player {};

};

int ParticleSystem::Setup() { return 0; }
void ParticleSystem::Init() {
    player.initGraphics();

    // Load the contents of the shaders
    std::string vertexShaderSource = readShaderFile("shaders/vertex_chunk.glsl");
    std::string fragmentShaderSource = readShaderFile("shaders/fragment_chunk.glsl");

    // Make sure they arent empty
    if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
        return;
    }

    // Create the shader program
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0) return;

    glCreateBuffers(1, &particlesColrosBuffer);
    auto particleColors = ParticleTypeSystem::GetParticleColorIndexesForShader();
    glNamedBufferStorage(
        particlesColrosBuffer,
        sizeof(NormColor) * particleColors.size(),
        (const void*)particleColors.data(),
        GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, particlesColrosBuffer);

    // ChunkConfig::EnableOutlines = false;

    for (unsigned x = 0; x < 2; ++x) {
        for (unsigned y = 0; y < 1; ++y) {
            for (unsigned z = 0; z < 2; ++z) {
                std::cout << "Init chunk at pos " << x << " " << y << " " << z << std::endl;
                particleChunks.push_back(
                    new ParticlesChunk(glm::ivec3(x, y, z), glm::uvec3(chunkSize))
                );
            }
        }
    }

    // todo: multithreading stuff
    chunksThread = std::jthread([](const std::stop_token& stoken) {
        while (true)
        {
            if (stoken.stop_requested()) {
                std::cout << "Stop requested for chunks thread" << std::endl;

                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(simulationStepInterval));

            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            chunksLock.lock();
            for (const auto& chunk : particleChunks) {
                // std::cout << chunk << std::endl;
                chunk->ProcessNextSimulationStep();
            }
            chunksLock.unlock();
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::chrono::duration<float> duration = end - start;
            simulationStepDelta = duration.count();
        }
    });

    std::cout << chunksThread.get_id() << std::endl;
}

void ParticleSystem::Update(float dt) {
    if (dt == 0.0f) return;

    // TODO: FIXME: It's not great having to pass this to the player update function
    player.update(dt, particleChunks);

    auto cameraChunkPosition = PositionConversion::WorldPositionToChunkPosition(CameraSystem::GetPosition() / particleScale, glm::uvec3(chunkSize));
    // std::cout << "Camera is in chunk " << cameraChunkPosition.x << " " << cameraChunkPosition.y << " " << cameraChunkPosition.z << std::endl;
    const int chunkDistance = 5; // Distance to load chunks around the player

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
            auto requiredChunkPosition = cameraChunkPosition + glm::ivec3(x, 0, z);

            // Enforce y to always be 0. We don't do vertical chunks right now
            requiredChunkPosition.y = 0;

            requiredChunkPositions.insert(requiredChunkPosition);
        }
    }

    // Manage chunks in one pass

    if (chunksLock.try_lock()) {
        std::vector<ParticlesChunk*> updatedChunks;

        for (auto& chunk : particleChunks) {
            auto pos = chunk->getGridPosition();
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
            auto* newChunk = new ParticlesChunk(pos, glm::uvec3(chunkSize));

            updatedChunks.push_back(newChunk);
        }

        particleChunks = std::move(updatedChunks);
        chunksLock.unlock();
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

    player.render();

    // Set the shader program
    glUseProgram(shaderProgram);


    auto projection = CameraSystem::GetProjection();
    auto view = CameraSystem::GetView();

    // todo: multiply these and send as one value to the GPU
    const GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    const GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    const GLint particleScaleLoc = glGetUniformLocation(shaderProgram, "particleScale");

    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform1f(particleScaleLoc, particleScale);

    // const GLint enableOutlinesLoc = glGetUniformLocation(shaderProgram, "EnableOutlines");
    // glUniform1ui(enableOutlinesLoc, ChunkConfig::EnableOutlines);

    for (const auto& chunk : particleChunks) {
        chunk->Render(window, shaderProgram, particlesColrosBuffer);
    }

    if (ImGui::Begin("Simulation Controls")) {
        ImGui::SliderInt("Simulation Step Interval", &simulationStepInterval, 0, 500);
        ImGui::SliderFloat("Particle Scale", &particleScale, 0.001f, 5.0f, "%.8f");
        // ImGui::Checkbox("Show Particle Outlines", &ChunkConfig::EnableOutlines);
    }
    ImGui::End();

    if (ImGui::Begin("Simulation Stats")) {
        ImGui::Text("Chunk Size %i", chunkSize);
        ImGui::Text("Simulation Step Delta %f", simulationStepDelta);
        ImGui::Text("Simulation Step Time Calculated %f", simulationStepInterval * simulationStepDelta);
    }
    ImGui::End();
}

void ParticleSystem::Exit() {
    chunksThread.request_stop();
    chunksThread.join();
}
void ParticleSystem::Done() {
    for (const auto& chunk : particleChunks) {
        delete chunk;
    }
}

/* Public Function Implementation */


/* Private Function Implementation */
