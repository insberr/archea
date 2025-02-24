//
// Created by jonah on 9/7/2024.
//


#pragma once
#include <mutex>
#include <queue>
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
    static bool EnableOutlines = false;
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

    void Update(float dt);
    void Render(GLFWwindow* window, GLuint shaderProgram, GLuint particlesColorsBuffer);

    void ProcessNextSimulationStep();

    // If the chunk has a lock on its data, this will block until it's done
    bool tryPlaceParticleAt(const glm::ivec3& worldParticlePosition, const ParticleData::ParticleInformation& particleInformation);

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

    // Simulation and Processing
    std::mutex lock;

    std::deque<glm::ivec3> nextPositionsToUpdate;

    // Rendering
    GLuint VBO {0}, VAO {0}, EBO {0};
    ChunkMesh chunkMesh { {}, {} };

    bool saveChunkData();
    bool loadChunkData();
    void remesh();
};
