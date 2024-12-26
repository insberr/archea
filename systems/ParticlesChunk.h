//
// Created by jonah on 9/7/2024.
//


#pragma once
#include <mutex>
#include <GL/glew.h>

#include "ParticleData.h"
#include <glm/glm.hpp>

#include "ImGuiSystem.h"
#include "InputSystem.h"

/*
 * What is a chunk responsible for doing?
 * A chunk keeps track of the particles within it: this means each chunk will have a ParticleData::Manager
 * A chunk is responsible for rendering the particles within it.
 * - It is not responsible for render order, or weather it should be rendered.
 * - Chunks should only render something when they are told to
 * A chunk must update the next state of particles when it is told to do so
 * A chunk is responsible for saving particle data on destruction
 * A chunk is responsible for loading particle data associated with it's position upon creation
 * Chunks need to be thread safe. They must be able to function within their own thread.
 * Reasons chunks would need to access another chunk:
 * - A particle moves outside of the chunk
 * ! Access to other chunks could be problematic for thread safety
 */

namespace PositionConversion
{
    glm::ivec3 WorldPositionToChunkPosition(const glm::vec3& worldPosition, const glm::uvec3& standardChunkSize);

    glm::ivec3 WorldPositionToParticleGrid(const glm::vec3& worldPosition, float standardParticleScale);

    glm::ivec3 ParticleGridToChunkGrid(const glm::ivec3& particlePosition, const glm::uvec3& standardChunkParticleGridSize);
}

namespace ChunkConfig {
    static int maxRaySteps = 300;
    static bool enableOutlines = false;
}

class ParticlesChunk {
public:
    ParticlesChunk(
        const glm::ivec3& chunkGridPosition,
        const glm::uvec3& particleGridSize,
        float particleScale
    );

    ~ParticlesChunk();

    // Update must have a really small workload
    bool Update(float dt);
    // todo: rename this?
    void ProcessNextSimulationStep();
    void Render(GLFWwindow* window, GLuint shaderProgram, GLuint particlesColorsBuffer, GLuint VAO);

    // If the chunk has a lock on it's data, this will block until it's done
    bool TryPlaceParticleAt(const glm::ivec3& worldParticlePosition, const ParticleData::DataWrapper& particleDataWraper);

    glm::vec3 getChunkDistanceFrom(const glm::ivec3& chunkPos);

    glm::ivec3 getChunkWorldPosition();
private:
    std::mutex lock; // I think this is right?

    std::jthread simulationThread;

    float particleScale;

    glm::vec3 worldPosition;
    glm::ivec3 chunkGridPosition;

    glm::vec3 worldChunkScale;
    glm::uvec3 chunkParticleGridSize;

    ParticleData::Manager particleManager;

    GLuint particlesBuffer;

    bool SaveChunkData();
    bool LoadChunkData();
};
