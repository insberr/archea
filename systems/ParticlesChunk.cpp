//
// Created by jonah on 9/7/2024.
//

#include "ParticlesChunk.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>  // for memcpy
#include <cstddef> // for std::byte
#include <queue>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "CameraSystem.h"
#include "InputSystem.h"
#include "Shapes.h"
#include "particle_types/ParticleMove.h"
#include "particle_types/ParticleType.h"
#include "particle_types/ParticleTypeSystem.h"

namespace PositionConversion
{
    glm::ivec3 WorldPositionToChunkPosition(
        const glm::vec3& worldPosition,
        const glm::uvec3& standardChunkSize
    ) {
        return worldPosition / glm::vec3(standardChunkSize);
    }

    glm::ivec3 WorldPositionToParticleGrid(
        const glm::vec3& worldPosition,
        float standardParticleScale
    ) {
        return worldPosition / standardParticleScale;
    }

    glm::ivec3 ParticleGridToChunkGrid(
        const glm::ivec3& particlePosition,
        const glm::uvec3& standardChunkParticleGridSize
    ) {
        return glm::floor(glm::vec3(particlePosition) / glm::vec3(standardChunkParticleGridSize));
    }
}

ParticlesChunk::ParticlesChunk(
    const glm::ivec3& chunkGridPosition,
    const glm::uvec3& dimensions
) : worldPosition(),
    gridPosition(chunkGridPosition),
    dimensions(dimensions)
{
    worldPosition = glm::vec3(chunkGridPosition) * static_cast<float>(dimensions.x); // TODO : VERY BAD

    // Set up vertex data and buffers and configure vertex attributes
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    if (!loadChunkData())
    {
        // todo: Temp, for testing
        for (unsigned x = 10; x < 14; ++x) {
            for (unsigned y = 5; y < 14; ++y) {
                for (unsigned z = 10; z < 14; ++z) {
                    const auto position = glm::uvec3(x, y, z);
                    particleHashMap.add(position, 2);
                    nextPositionsToUpdate.emplace_back(position);
                }
            }
        }
    }

    remesh();

    // todo: save/load update queue rather than adding everything
    // // Add all non-air to queue for update
    // for (unsigned x = 0; x < chunkParticleGridSize.x; ++x) {
    //     for (int y = 0; y < chunkParticleGridSize.y; ++y) {
    //         for (unsigned z = 0; z < chunkParticleGridSize.z; ++z) {
    //             const auto currentPos = glm::ivec3(x, y, z);
    //
    //             if (!particleManager.Exists(currentPos)) {
    //                 continue;
    //             }
    //
    //             nextPositionsToUpdate.push_back(currentPos);
    //         }
    //     }
    // }

    // Should probably use a thread pool and pass these in as scheduled work or something
//    using namespace std::chrono_literals;
//    static auto stepDelay = 1000ms;
//    simulationThread = std::jthread([this]() {
//        while (true) {
//            this->ProcessNextSimulationStep();
//
//            // const auto start = std::chrono::high_resolution_clock::now();
//            std::this_thread::sleep_for(stepDelay);
//            // const auto end = std::chrono::high_resolution_clock::now();
//            // const std::chrono::duration<double, std::milli> elapsed = end - start;
//
//            // std::cout << "Waited " << elapsed.count() << '\n';
//        }
//    });
}

ParticlesChunk::~ParticlesChunk() {
    const std::lock_guard guard(lock);
    saveChunkData();
}

void ParticlesChunk::Update(float dt) {
    if (dt == 0.0f) return;

    // todo: use this to load/unload chunks
    // const glm::ivec3 currentChunk = PositionConversion::WorldPositionToChunkPosition(CameraSystem::GetPosition(), chunkParticleGridSize);
    // if (glm::distance(glm::vec3(currentChunk), glm::vec3(chunkGridPosition)) >= 2.0f) {
    //     // Do something
    // }
}

void ParticlesChunk::ProcessNextSimulationStep() {
    const std::lock_guard guard(lock);

    if (nextPositionsToUpdate.empty()) {
        return;
    }

    const auto positionsToUpdate = std::move(nextPositionsToUpdate);

    for (const auto& currentPos : positionsToUpdate) {
        if (!particleHashMap.exists(currentPos)) {
            continue;
        }

        // z * (ysize * xsize) + y * (xsize) + x
        unsigned particle = particleHashMap.get(currentPos).particleType;
        // if (particle <= 1) continue;

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
            posToTry = glm::clamp(posToTry, glm::ivec3(0), glm::ivec3(dimensions - glm::uvec3(1)));
            // int newY = std::clamp<int>(y - 1, 0, 49);

            unsigned atNewPos = particleHashMap.get(posToTry).particleType;
            if (atNewPos != 0) continue;

            // We are here, we are allowed to move here

            particleHashMap.add(posToTry, particleHashMap.get(currentPos));
            particleHashMap.remove(currentPos);

            const auto posBelowTryPos = posToTry - glm::ivec3(0, 1, 0);
            try {
                unsigned atPosBelowTryPos = particleHashMap.get(posBelowTryPos).particleType;
                if (atPosBelowTryPos == 0) {
                    nextPositionsToUpdate.push_back(posToTry);
                }
            } catch (...) {
                // do nothing. we are out of bounds so the particle is on the floor...
            }

            // Add particle above currentPos to queue
            const auto aboveCurrentPos = currentPos + glm::ivec3(0, 1, 0);
            unsigned atPosAboveCurrentPos = particleHashMap.get(aboveCurrentPos).particleType;
            if (atPosAboveCurrentPos != 0) {
                nextPositionsToUpdate.push_back(aboveCurrentPos);
            }
            break;
            // if works, break
            // else continue
        }
        //end
    }

    remesh();
}

void ParticlesChunk::Render(
    GLFWwindow* window,
    GLuint shaderProgram,
    GLuint particlesColorsBuffer
) {
    glBindVertexArray(VAO);

    if (lock.try_lock()) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, chunkMesh.vertices.size() * sizeof(float), chunkMesh.vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunkMesh.indicies.size() * sizeof(unsigned int), chunkMesh.indicies.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        lock.unlock();
    }

    const GLint worldPositionLoc = glGetUniformLocation(shaderProgram, "worldPosition");
    glUniform3f(worldPositionLoc, worldPosition.x, worldPosition.y, worldPosition.z);

    glDrawElements(GL_TRIANGLES, chunkMesh.indicies.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

bool ParticlesChunk::tryPlaceParticleAt(
    const glm::ivec3& worldParticlePosition,
    const ParticleData::ParticleInformation& particleInformation
)
{
    const glm::ivec3 chunkPos = PositionConversion::ParticleGridToChunkGrid(worldParticlePosition, dimensions);
    if (chunkPos != gridPosition) {
        return false;
    }

    // Block until able to acquire lock
    const std::lock_guard guard(lock);

    const glm::uvec3 particlePositionInChunk = glm::abs(worldParticlePosition - (gridPosition * glm::ivec3(dimensions)));

    auto& particle = particleHashMap.get(particlePositionInChunk);
    if (particleInformation.particleType == particle.particleType) return false;
    particle.particleType = particleInformation.particleType;
    nextPositionsToUpdate.emplace_back(particlePositionInChunk);

    remesh();

    return false;
}

glm::ivec3 ParticlesChunk::getGridPosition() const {
    return gridPosition;
}
glm::ivec3 ParticlesChunk::getWorldPosition() const {
    return worldPosition;
}

bool ParticlesChunk::saveChunkData()
{
    // std::stringstream filenameForChunk;
    // filenameForChunk
    // << "chunks/"
    // << gridPosition.x
    // << "-"
    // << gridPosition.y
    // << "-"
    // << gridPosition.z
    // << ".bin";
    //
    // // TODO: Create chunks folder if it does not exist
    //
    // std::vector<std::byte> bytes(sizeof(unsigned) * particleHashMap.GetParticleTypesData().size());
    // std::memcpy(bytes.data(), particleHashMap.GetParticleTypesData().data(), sizeof(unsigned) * particleHashMap.GetParticleTypesData().size());
    //
    // std::ofstream file(filenameForChunk.str(), std::ios::binary);
    // if (file) {
    //     file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    // }
    // file.close();

    return true;
}

bool ParticlesChunk::loadChunkData()
{
    return false;
    // try {
    //     std::stringstream filenameForChunk;
    //     filenameForChunk
    //     << "chunks/"
    //     << chunkGridPosition.x
    //     << "-"
    //     << chunkGridPosition.y
    //     << "-"
    //     << chunkGridPosition.z
    //     << ".bin";
    //
    //     std::ifstream file(filenameForChunk.str(), std::ios::binary);
    //
    //     if (file) {
    //         std::cout << "Reading: " << filenameForChunk.str() << std::endl;
    //
    //         file.read(
    //             reinterpret_cast<char*>( const_cast<unsigned*>(particleHashMap.GetParticleTypesData().data()) ),
    //             sizeof(unsigned) * 64 * 64 * 64
    //         );
    //     } else {
    //         file.close();
    //         return false;
    //     }
    //     file.close();
    //
    //     return true;
    // } catch (std::exception& e) {
    //     return false;
    // }
}

void ParticlesChunk::remesh() {
    chunkMesh.vertices.clear();
    chunkMesh.indicies.clear();

    int i = 0;
    for (const auto& [position, data] : particleHashMap.getAll()) {
        // todo: this should never happen
        if (data.particleType == 0) continue;

        for (int j = 0; j < sizeof(Shapes::Cube::cubeVertices) / sizeof(float); j += 6) {
            chunkMesh.vertices.push_back(Shapes::Cube::cubeVertices[j] + position.x);
            chunkMesh.vertices.push_back(Shapes::Cube::cubeVertices[j + 1] + position.y);
            chunkMesh.vertices.push_back(Shapes::Cube::cubeVertices[j + 2] + position.z);

            chunkMesh.vertices.push_back(Shapes::Cube::cubeVertices[j + 3]);
            chunkMesh.vertices.push_back(Shapes::Cube::cubeVertices[j + 4]);
            chunkMesh.vertices.push_back(Shapes::Cube::cubeVertices[j + 5]);
        }

        int vertexOffset = i * 24;
        for (unsigned int ind : Shapes::Cube::indices) {
            chunkMesh.indicies.push_back(ind + vertexOffset);
        }
        i++;
    }
}
