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

#include <mdspan>
#include <array>
#include <unordered_map>

namespace ParticlesDataManager {
    // Custom hash function for glm::ivec3
    struct IVec3Hash {
        std::size_t operator()(const glm::ivec3& v) const noexcept {
            // Combine the hash values of the three integers
            std::size_t h1 = std::hash<int>()(v.x);
            std::size_t h2 = std::hash<int>()(v.y);
            std::size_t h3 = std::hash<int>()(v.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2); // Bit-shifting to reduce collisions
        }
    };

    struct Bounds {
        glm::ivec3 min;
        glm::ivec3 max;
    };

    class Chunk {
        using size_max_extents = std::extents<size_t, SIZE_MAX, SIZE_MAX, SIZE_MAX>;
        public:
        Chunk(const unsigned int chunkSize, const Bounds& bounds, const std::unordered_map<glm::ivec3, unsigned int, IVec3Hash>& map)
            : chunkSize(chunkSize), bounds(bounds), particlesMap(&map)
        {

        }

        std::vector<unsigned int> getAllParticles() {
            std::vector<unsigned int> particles(chunkSize * chunkSize * chunkSize);
            std::mdspan<unsigned int, size_max_extents> particlesView = std::mdspan(particles.data(), chunkSize, chunkSize, chunkSize);

            // todo: find a more efficient way to do this
            for (int i = bounds.min.x; i <= bounds.max.x; i++) {
                for (int j = bounds.min.y; j <= bounds.max.y; j++) {
                    for (int k = bounds.min.z; k <= bounds.max.z; k++) {
                        particlesView[std::array<int, 3>{i, j, k}] = particlesMap->at(glm::ivec3{i, j, k});
                    }
                }
            }

            return std::move(particles);
        }

        std::unordered_map<glm::ivec3, unsigned int, IVec3Hash> getParticlesAsMap() {
            std::unordered_map<glm::ivec3, unsigned int, IVec3Hash> particles;

            for (int i = bounds.min.x; i <= bounds.max.x; i++) {
                for (int j = bounds.min.y; j <= bounds.max.y; j++) {
                    for (int k = bounds.min.z; k <= bounds.max.z; k++) {
                        const auto pos = glm::ivec3{i, j, k};
                        particles.insert(std::make_pair(pos, particlesMap->at(pos)));
                    }
                }
            }

            return std::move(particles);
        }

    private:
        unsigned int chunkSize;
        Bounds bounds;
        const std::unordered_map<glm::ivec3, unsigned int, IVec3Hash>* particlesMap;
    };

    // todo: type alias?
    // using MyHashMap = std::unordered_map<glm::ivec3, unsigned int, IVec3Hash>;

    class Particles {
        public:
        Particles(unsigned int chunkSize, const Bounds& bounds) :
            chunkSize(chunkSize)
        {

        }
        ~Particles() {}

        Chunk getChunk(const glm::ivec3& position) {
            const Chunk chunk(chunkSize, {position, position + glm::ivec3(chunkSize - 1)}, particlesMap);
            return chunk;
        }

        // FIXME: Not a great name. Basically this is just for loading in particles
        void insertParticles(const ::std::unordered_map<glm::ivec3, unsigned int, IVec3Hash>& particles_mapping) {
            // FIXME: There's probably a better way to essentially combine two maps...
            for (const auto& point : particles_mapping) {
                auto position = point.first;
                auto value = point.second;
                particlesMap.insert(std::make_pair(position, value));
            }
        }

        private:
        std::unordered_map<glm::ivec3, unsigned int, IVec3Hash> particlesMap;

        unsigned int chunkSize;
    };
}

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

    // ParticleData
    ParticleData::Manager particleDataManager{ glm::uvec3(chunkSize) };

    // Chunks
    std::vector<ParticlesChunk*> particleChunks;
    std::mutex chunksLock;
    std::jthread chunksThread;



    // Demo new particle data thing
    // 3x3 of chunks basically
    ParticlesDataManager::Particles particlesData { chunkSize, { glm::ivec3(0), glm::ivec3(chunkSize * 3)}};

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
    ChunkConfig::maxRaySteps = 300;

    for (unsigned x = 0; x < 2; ++x) {
        for (unsigned y = 0; y < 1; ++y) {
            for (unsigned z = 0; z < 2; ++z) {
                std::cout << "Init chunk at pos " << x << " " << y << " " << z << std::endl;
                ParticleData::Bounds bounds = {
                    .min = glm::ivec3(glm::uvec3(chunkSize) * glm::uvec3(x, y, z)),
                    // todo: is the -1 needed???
                    .max = glm::ivec3(glm::uvec3(chunkSize) * glm::uvec3(x + 1, y + 1, z + 1)) - glm::ivec3(1),
                };

                ParticleData::Chunk* dataChunk = particleDataManager.getChunk(bounds.min, chunkSize);

                particleChunks.push_back(
                    new ParticlesChunk(dataChunk, glm::ivec3(x, y, z), glm::uvec3(chunkSize), particleScale)
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

            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            if (chunksLock.try_lock()) {
                for (const auto& chunk : particleChunks) {
                    // std::cout << chunk << std::endl;
                    chunk->ProcessNextSimulationStep();
                }
                chunksLock.unlock();
            }
        }
    });

    std::cout << chunksThread.get_id() << std::endl;
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
                { lookingAtParticlePos, drawType, 32.0f }
            );

            std::unordered_map<glm::ivec3, unsigned int, ParticlesDataManager::IVec3Hash> particlesMap;
            particlesMap[lookingAtParticlePos] = drawType;
            particlesData.insertParticles(particlesMap);
        }
    }

    if (InputSystem::IsMouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT)) {
        // particleDataManager.SetType(drawPos, 0);
        for (const auto& chunk : particleChunks) {
            chunk->TryPlaceParticleAt(
                lookingAtParticlePos,
                { lookingAtParticlePos, 0,0.0f }
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
            // todo: update this
            // auto* newChunk = new ParticlesChunk(pos, glm::uvec3(chunkSize), particleScale);
            //
            // updatedChunks.push_back(newChunk);
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

    // // TEMP testing
    // ParticlesDataManager::Chunk chunk = particlesData.getChunk(glm::ivec3(0));
    // for (const auto& particle : chunk.getParticlesAsMap()) {
    //     auto pos = particle.first;
    //     std::cout << "Particle in chunk 0,0,0 at position " << pos.x << "," << pos.y << "," << pos.z << ": " << particle.second << std::endl;
    // }

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
