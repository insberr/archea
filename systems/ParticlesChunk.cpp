//
// Created by jonah on 9/7/2024.
//

#include "ParticlesChunk.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
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


    if (!loadChunkData() && chunkGridPosition.y == 0)
    {
        // todo: Temp, for testing
        for (unsigned x = 0; x < 64; ++x) {
            for (unsigned y = 0; y < 1; ++y) {
                for (unsigned z = 0; z < 64; ++z) {
                    const auto position = glm::uvec3(x, y, z);
                    particleHashMap.add(position, 2);
                    // nextPositionsToUpdate.emplace_back(position);
                }
            }
        }
    }

    particleHashMap.add(glm::ivec3(0), 1);
    particleHashMap.add(glm::ivec3(0, 0, 1), 1);
    particleHashMap.add(glm::ivec3(0, 1, 0), 1);
    particleHashMap.add(glm::ivec3(1, 0, 0), 1);

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

void ParticlesChunk::InitGraphics() {
    // Set up vertex data and buffers and configure vertex attributes
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void ParticlesChunk::Update(float dt) {
    if (dt == 0.0f) return;

    // todo: use this to load/unload chunks
    // const glm::ivec3 currentChunk = PositionConversion::WorldPositionToChunkPosition(CameraSystem::GetPosition(), chunkParticleGridSize);
    // if (glm::distance(glm::vec3(currentChunk), glm::vec3(chunkGridPosition)) >= 2.0f) {
    //     // Do something
    // }
}

void ParticlesChunk::ProcessNextSimulationStep(std::unordered_map<glm::ivec3, ParticlesChunk*>& particlesChunks) {
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
        ParticleData::ParticleInformation& particle = particleHashMap.get(currentPos);
        // if (particle <= 1) continue;

        ParticleType& particleTypeInfo = ParticleTypeSystem::GetParticleTypeInfo(particle.particleType);

        auto nextMove = ParticleMove::MoveState {};
        while (true) {
            glm::ivec3 posToTry;

            particleTypeInfo.getNextMove(nextMove);

            if (nextMove.done) break;

            posToTry = currentPos + glm::ivec3(nextMove.positionToTry);

            // try pos

            // if (posToTry.y < 0) continue;

            // posToTry = glm::clamp(posToTry, glm::ivec3(0), glm::ivec3(dimensions - glm::uvec3(1)));
            // int newY = std::clamp<int>(y - 1, 0, 49);
            glm::ivec3 boundsCmp = glm::ivec3(dimensions - glm::uvec3(1)) - posToTry;

            glm::ivec3 chunkPosInside = gridPosition;
            glm::uvec3 posToTryInNewChunk = posToTry;

            if (boundsCmp.x < 0) {
                chunkPosInside.x += 1;
                posToTryInNewChunk.x -= dimensions.x;
            } else if (boundsCmp.x >= dimensions.x) {
                chunkPosInside.x -= 1;
                posToTryInNewChunk.x += dimensions.x;
            }

            if (boundsCmp.y < 0) {
                chunkPosInside.y += 1;
                posToTryInNewChunk.y -= dimensions.y;
            } else if (boundsCmp.y >= dimensions.y) {
                chunkPosInside.y -= 1;
                posToTryInNewChunk.y += dimensions.y;
            }

            if (boundsCmp.z < 0) {
                chunkPosInside.z += 1;
                posToTryInNewChunk.z -= dimensions.z;
            } else if (boundsCmp.z >= dimensions.z) {
                chunkPosInside.z -= 1;
                posToTryInNewChunk.z += dimensions.z;
            }

            if (chunkPosInside != gridPosition) {
                if (!particlesChunks.contains(chunkPosInside)) {
                    continue;
                }

                const bool moved = particlesChunks.at(chunkPosInside)->tryMoveParticleTo(
                    posToTryInNewChunk,
                    particleHashMap.get(currentPos)
                );

                // if we moved, remove from us and break
                if (moved) {
                    particleHashMap.remove(currentPos);
                    break;
                }

                // if we didn't, continue
                continue;
            }


            if (particleHashMap.exists(posToTry)) continue;

            // We are here, we are allowed to move here

            ParticleData::ParticleInformation info = particleHashMap.get(currentPos);
            particleHashMap.add(posToTry, info);
            particleHashMap.remove(currentPos);

            const auto posBelowTryPos = posToTry - glm::ivec3(0, 1, 0);
            if (!particleHashMap.exists(posBelowTryPos)) {
                nextPositionsToUpdate.insert(posToTry);
            }
            // Add particle above currentPos to queue
            const auto aboveCurrentPos = currentPos + glm::ivec3(0, 1, 0);
            if (particleHashMap.exists(aboveCurrentPos)) {
                nextPositionsToUpdate.insert(aboveCurrentPos);
            }
            break;
            // if works, break
            // else continue
        }
        //end
    }

    particlesDirty.store(true);
}

void ParticlesChunk::ProcessPostSimulationStep() {
    nextPositionsToUpdate.merge(futureNextPositionsToUpdate);
    futureNextPositionsToUpdate.clear();

    if (particlesDirty.load()) {
        remesh();
        particlesDirty.store(false);
    }
}

bool ParticlesChunk::tryMoveParticleTo(
    const glm::uvec3& positionToMoveTo,
    const ParticleData::ParticleInformation& particleInformation
) {
    if (particleHashMap.exists(positionToMoveTo)) {
        return false;
    }

    particleHashMap.add(positionToMoveTo, particleInformation);
    futureNextPositionsToUpdate.insert(positionToMoveTo);

    particlesDirty.store(true);

    return true;
}

void ParticlesChunk::Render(
    GLFWwindow* window,
    GLuint shaderProgram,
    GLuint particlesColorsBuffer
) {
    glBindVertexArray(VAO);

    if (meshDirty.load()) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, chunkMesh.vertices.size() * sizeof(float), chunkMesh.vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunkMesh.indicies.size() * sizeof(unsigned int), chunkMesh.indicies.data(), GL_STATIC_DRAW);
        meshDirty.store(false);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));

    const GLint worldPositionLoc = glGetUniformLocation(shaderProgram, "worldPosition");
    glUniform3f(worldPositionLoc, worldPosition.x, worldPosition.y, worldPosition.z);

    glDrawElements(GL_TRIANGLES, chunkMesh.indicies.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

void ParticlesChunk::tryPlaceParticleAt(
    const glm::ivec3& worldParticlePosition,
    const ParticleData::ParticleInformation& particleInformation
)
{
    // Block until able to acquire lock
    const std::lock_guard guard(lock);

    const glm::uvec3 particlePositionInChunk = glm::abs(worldParticlePosition - (gridPosition * glm::ivec3(dimensions)));

    if (particleInformation.particleType == 0) {
        particleHashMap.remove(particlePositionInChunk);
        // todo: might be good to remove from nextPositionsToUpdate too
        particlesDirty.store(true);
        return;
    }

    bool doRemesh = false;
    if (particleHashMap.exists(particlePositionInChunk)) {
        ParticleData::ParticleInformation& particle = particleHashMap.get(particlePositionInChunk);
        if (particle.particleType != particleInformation.particleType) {
            doRemesh = true;
        }
        particle = particleInformation;
    } else {
        particleHashMap.add(particlePositionInChunk, particleInformation);
        doRemesh = true;
    }
    nextPositionsToUpdate.emplace(particlePositionInChunk);

    if (doRemesh) {
        particlesDirty.store(true);
    }
}

glm::ivec3 ParticlesChunk::getGridPosition() const {
    return gridPosition;
}
glm::ivec3 ParticlesChunk::getWorldPosition() const {
    return worldPosition;
}

// TODO: Move this somewhere else and rename it to something better
const unsigned separator = 0xffffffff;

bool ParticlesChunk::saveChunkData()
{
    namespace fs = std::filesystem;

    std::stringstream filenameForChunk;
    filenameForChunk
    << "chunks/"
    << gridPosition.x
    << "-"
    << gridPosition.y
    << "-"
    << gridPosition.z
    << ".bin";

    fs::path path(filenameForChunk.str());
    fs::create_directories(path.parent_path());

    std::ofstream file(filenameForChunk.str(), std::ios::binary);
    if (file) {
        file.write(reinterpret_cast<char*>(&dimensions), sizeof(dimensions));
        file.write(reinterpret_cast<const char*>(&separator), sizeof(separator));

        file.write(reinterpret_cast<char*>(&gridPosition), sizeof(gridPosition));
        file.write(reinterpret_cast<const char*>(&separator), sizeof(separator));

        file.write(reinterpret_cast<const char*>(&separator), sizeof(separator));
        file.write(reinterpret_cast<const char*>(&separator), sizeof(separator));
        file.write(reinterpret_cast<const char*>(&separator), sizeof(separator));
        file.write(reinterpret_cast<const char*>(&separator), sizeof(separator));
        for (glm::ivec3 posInDeque : nextPositionsToUpdate) {
            file.write(reinterpret_cast<char*>(&posInDeque), sizeof(glm::ivec3));
            file.write(reinterpret_cast<const char*>(&separator), sizeof(separator));
        }

        file.write(reinterpret_cast<const char*>(&separator), sizeof(separator));
        file.write(reinterpret_cast<const char*>(&separator), sizeof(separator));
        file.write(reinterpret_cast<const char*>(&separator), sizeof(separator));
        file.write(reinterpret_cast<const char*>(&separator), sizeof(separator));
        for (auto [position, particleInfo] : particleHashMap.getAll()) {
            file.write(reinterpret_cast<const char*>(&position), sizeof(glm::ivec3));
            file.write(reinterpret_cast<char*>(&particleInfo.particleType), sizeof(unsigned));
        }
    }
    file.close();

    return true;
}

bool ParticlesChunk::loadChunkData()
{
    try {
        std::stringstream filenameForChunk;
        filenameForChunk
        << "chunks/"
        << gridPosition.x
        << "-"
        << gridPosition.y
        << "-"
        << gridPosition.z
        << ".bin";

        std::ifstream file(filenameForChunk.str(), std::ios::binary);

        if (file) {
            // std::cout << "Reading: " << filenameForChunk.str() << std::endl;

            file.read(reinterpret_cast<char*>(&dimensions), sizeof(dimensions));
            file.seekg(sizeof(separator), std::ios::cur);

            file.read(reinterpret_cast<char*>(&gridPosition), sizeof(gridPosition));
            file.seekg(sizeof(separator), std::ios::cur);

            file.seekg(sizeof(separator) * 4, std::ios::cur);

            // Read an ivec3 and a separator until an ivec3 of separator
            while (!file.eof()) {
                glm::ivec3 positionInDeque;

                file.read(reinterpret_cast<char*>(&positionInDeque), sizeof(positionInDeque));
                file.seekg(sizeof(separator), std::ios::cur);

                if (positionInDeque == glm::ivec3(glm::uvec3(separator))) break;
                nextPositionsToUpdate.emplace(positionInDeque);
            }

            while (!file.eof()) {
                glm::ivec3 particlePosition;
                ParticleData::ParticleInformation particleInformation { 0, 32.0f };

                file.read(reinterpret_cast<char*>(&particlePosition), sizeof(particlePosition));
                file.read(reinterpret_cast<char*>(&particleInformation.particleType), sizeof(particleInformation.particleType));
                file.peek();

                particleHashMap.add(particlePosition, particleInformation);
            }
        } else {
            file.close();
            return false;
        }
        file.close();
        return true; // todo: TEMP: return true;

    } catch (std::exception& e) {
        return false;
    }
}

void ParticlesChunk::remesh() {
    ChunkMesh newChunkMesh;

    int indicesSum = 0;
    for (const auto& [position, data] : particleHashMap.getAll()) {
        // todo: this should never happen
        if (data.particleType == 0) {
            // std::cout << "ParticlesChunk::remesh: particleType is 0" << std::endl;
            //continue;
        }

        int facesDrawn = 0;
        for (int face = 0; face < 6; ++face) {
            int normalOffset = face * 3;

            const glm::ivec3 positionNextToFace = position + glm::ivec3(
                Shapes::Cube::cubeNormals[normalOffset],
                Shapes::Cube::cubeNormals[normalOffset + 1],
                Shapes::Cube::cubeNormals[normalOffset + 2]
            );

            bool faceCulled = false;
            if (particleHashMap.exists(positionNextToFace)) {
                faceCulled = true;
                continue;
            }

            // Add the vertices
            const int verticesOffset = face * 12;
            for (int vertex = 0; vertex < 12; vertex += 3) {
                const int v = verticesOffset + vertex;
                newChunkMesh.vertices.push_back(Shapes::Cube::cubeVertices[v] + position.x);
                newChunkMesh.vertices.push_back(Shapes::Cube::cubeVertices[v + 1] + position.y);
                newChunkMesh.vertices.push_back(Shapes::Cube::cubeVertices[v + 2] + position.z);

                newChunkMesh.vertices.push_back(Shapes::Cube::cubeNormals[normalOffset]);
                newChunkMesh.vertices.push_back(Shapes::Cube::cubeNormals[normalOffset + 1]);
                newChunkMesh.vertices.push_back(Shapes::Cube::cubeNormals[normalOffset + 2]);

                newChunkMesh.vertices.push_back(data.particleType);
            }

            // Add indices
            int indicesOffset = indicesSum;
            int indicesFaceOffset = facesDrawn * 6;
            for (int ind = 0; ind < 6; ++ind) {
                newChunkMesh.indicies.push_back(Shapes::Cube::indices[indicesFaceOffset + ind] + indicesOffset);
            }

            facesDrawn++;
        }
        indicesSum += facesDrawn * 4;
    }

    chunkMesh = newChunkMesh;
    meshDirty.store(true);
}
