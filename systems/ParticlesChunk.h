//
// Created by jonah on 9/7/2024.
//


#pragma once
#include <atomic>
#include <mutex>
#include <queue>
#include <unordered_set>
#include <GL/glew.h>

#include "ParticleData.h"
#include <glm/glm.hpp>

#include "InputSystem.h"

namespace PositionConversion
{
    glm::ivec3 WorldPositionToChunkPosition(const glm::vec3& worldPosition, const glm::uvec3& standardChunkSize);

    glm::ivec3 WorldPositionToParticleGrid(const glm::vec3& worldPosition, float standardParticleScale);

    glm::ivec3 ParticleGridToChunkGrid(const glm::ivec3& particlePosition, const glm::uvec3& standardChunkParticleGridSize);
}

namespace ChunkConfig {
    // static bool EnableOutlines = false;
    static float ParticleScale = 1.0f;
}

// todo: move somewhere else
struct ChunkMesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indicies;
};

class ParticlesChunk {
public:
    ParticlesChunk(
        const glm::ivec3& chunkGridPosition,
        const glm::uvec3& dimensions
    );

    ~ParticlesChunk();

    void InitGraphics();
    void Update(float dt);
    void Render(GLFWwindow* window, GLuint shaderProgram, GLuint particlesColorsBuffer);

    void ProcessNextSimulationStep(std::unordered_map<glm::ivec3, ParticlesChunk*>& particlesChunks);
    void ProcessPostSimulationStep();

    bool tryMoveParticleTo(const glm::uvec3& positionToMoveTo, const ParticleData::ParticleInformation& particleInformation);
    // If the chunk has a lock on its data, this will block until it's done
    void tryPlaceParticleAt(const glm::ivec3& worldParticlePosition, const ParticleData::ParticleInformation& particleInformation);

    glm::ivec3 getGridPosition() const;
    glm::ivec3 getWorldPosition() const;
private:
    // Spacial Information
    glm::vec3 worldPosition;
    glm::ivec3 gridPosition;
    // glm::vec3 worldChunkScale;
    // todo: make const
    glm::uvec3 dimensions;

    // Data
    ParticleData::ParticleHashMap particleHashMap;
    std::atomic<bool> particlesDirty = false;

    // Simulation and Processing
    std::mutex lock;

    std::unordered_set<glm::ivec3> nextPositionsToUpdate;
    std::unordered_set<glm::ivec3> futureNextPositionsToUpdate;

    // Rendering
    GLuint VBO {0}, VAO {0}, EBO {0};
    ChunkMesh chunkMesh { {}, {} };
    std::atomic<bool> meshDirty = false;

    bool saveChunkData();
    bool loadChunkData();
    void remesh();
};
