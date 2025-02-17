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
    ParticleData::Chunk const* particleDataChunk,
    const glm::ivec3& chunkGridPosition,
    const glm::uvec3& particleGridSize,
    float particleScale
) : particleScale(particleScale),
    worldPosition(),
    chunkGridPosition(chunkGridPosition),
    worldChunkScale(),
    chunkParticleGridSize(particleGridSize),
    particleDataChunk(particleDataChunk)
{
    worldChunkScale = glm::vec3(particleGridSize) * particleScale;
    worldPosition = glm::vec3(chunkGridPosition) * worldChunkScale;

    glCreateBuffers(1, &particlesBuffer);

    const glm::ivec3 chunkDimensions = particleDataChunk->getDimensions();
    const int chunkCubicSize = chunkDimensions.x * chunkDimensions.y * chunkDimensions.z;

    glNamedBufferStorage(
        particlesBuffer,
        sizeof(int) * chunkCubicSize,
        static_cast<const void*>(particleDataChunk->asParticleTypes3DArray().data()),
        GL_DYNAMIC_STORAGE_BIT
    );

    if (!LoadChunkData())
    {
        // todo: Temp, for testing
        for (unsigned x = 10; x < 14; ++x) {
            for (unsigned y = 5; y < 14; ++y) {
                for (unsigned z = 10; z < 14; ++z) {
                    const glm::ivec3 position = glm::ivec3(x, y, z) + particleDataChunk->getBounds().min;
                    particleDataChunk->addParticle(position, {
                        position,
                        2,
                        32.0f
                    });
                }
            }
        }
    }

    // Add all non-air to queue for update
    for (const auto& [position, particleInfo] : particleDataChunk->asClonedMap()) {
        nextPositionsToUpdate.push_back(position);
    }

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
    SaveChunkData();
}

bool ParticlesChunk::Update(float dt) {
    if (dt == 0.0f) return true;

    // todo: use this to load/unload chunks
    // const glm::ivec3 currentChunk = PositionConversion::WorldPositionToChunkPosition(CameraSystem::GetPosition(), chunkParticleGridSize);
    // if (glm::distance(glm::vec3(currentChunk), glm::vec3(chunkGridPosition)) >= 2.0f) {
    //     // Do something
    // }

    return true;
}

void ParticlesChunk::ProcessNextSimulationStep() {
    // const std::lock_guard guard(lock);
    //
    // if (nextPositionsToUpdate.empty()) {
    //     return;
    // }
    //
    // const auto positionsToUpdate = std::move(nextPositionsToUpdate);
    //
    // for (const auto& currentPos : positionsToUpdate) {
    //     if (!particleManager.Exists(currentPos)) {
    //         continue;
    //     }
    //
    //     // z * (ysize * xsize) + y * (xsize) + x
    //     unsigned particle = particleManager.Get(currentPos).particleType;
    //     // if (particle <= 1) continue;
    //
    //     auto particleTypeInfo = ParticleTypeSystem::GetParticleTypeInfo(particle - 1);
    //
    //     auto posToTry = glm::ivec3(0);
    //
    //     // new
    //     auto nextMove = ParticleMove::MoveState {};
    //     while (true) {
    //         particleTypeInfo.getNextMove(nextMove);
    //
    //         if (nextMove.done) break;
    //
    //         posToTry = currentPos + glm::ivec3(nextMove.positionToTry);
    //
    //         // try pos
    //
    //         if (posToTry.y < 0) continue;
    //         posToTry = glm::clamp(posToTry, glm::ivec3(0), glm::ivec3(chunkParticleGridSize - glm::uvec3(1)));
    //         // int newY = std::clamp<int>(y - 1, 0, 49);
    //
    //         unsigned atNewPos = particleManager.Get(posToTry).particleType;
    //         if (atNewPos != 0) continue;
    //
    //         // We are here, we are allowed to move here
    //
    //         particleManager.SetType(posToTry, particle);
    //         // remove
    //         particleManager.SetType(currentPos, 0);
    //
    //         const auto posBelowTryPos = posToTry - glm::ivec3(0, 1, 0);
    //         try {
    //             unsigned atPosBelowTryPos = particleManager.Get(posBelowTryPos).particleType;
    //             if (atPosBelowTryPos == 0) {
    //                 nextPositionsToUpdate.push_back(posToTry);
    //             }
    //         } catch (...) {
    //             // do nothing. we are out of bounds so the particle is on the floor...
    //         }
    //
    //         // Add particle above currentPos to queue
    //         const auto aboveCurrentPos = currentPos + glm::ivec3(0, 1, 0);
    //         unsigned atPosAboveCurrentPos = particleManager.Get(aboveCurrentPos).particleType;
    //         if (atPosAboveCurrentPos != 0) {
    //             nextPositionsToUpdate.push_back(aboveCurrentPos);
    //         }
    //         break;
    //         // if works, break
    //         // else continue
    //     }
    //     //end
    // }
}

void ParticlesChunk::Render(
    GLFWwindow* window,
    GLuint shaderProgram,
    GLuint particlesColorsBuffer,
    GLuint VAO
) {
    const glm::ivec3 chunkDimensions = particleDataChunk->getDimensions();
    const int chunkCubicSize = chunkDimensions.x * chunkDimensions.y * chunkDimensions.z;

    glNamedBufferSubData(
        particlesBuffer,
        0,
        sizeof(int) * chunkCubicSize,
        static_cast<const void*>(particleDataChunk->asParticleTypes3DArray().data())
    );

    // Bind the particles data
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particlesBuffer);
    // Bind the particles color data
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particlesColorsBuffer);

    // Set the shader program
    glUseProgram(shaderProgram);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    // glUniform2f(glGetUniformLocation(shaderProgram, "Resolution"), width, height);
    glUniform1f(glGetUniformLocation(shaderProgram, "Time"), static_cast<float>(glfwGetTime()));

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
    auto model = glm::translate(CameraSystem::GetModel(), worldPosition);
    auto view = CameraSystem::GetView();
    auto projection = CameraSystem::GetProjection();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform1i(glGetUniformLocation(shaderProgram, "MAX_RAY_STEPS"), ChunkConfig::maxRaySteps);
    glUniform1f(glGetUniformLocation(shaderProgram, "ParticleScale"), particleScale);
    glUniform1ui(glGetUniformLocation(shaderProgram, "EnableOutlines"), ChunkConfig::enableOutlines);
    // Fixme, chunksize xyz is no longer garenteed to be the same
    glUniform1ui(glGetUniformLocation(shaderProgram, "ChunkSize"), chunkParticleGridSize.x);

    // Bind the vertex data
    glBindVertexArray(VAO);
    // Call draw
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}

bool ParticlesChunk::TryPlaceParticleAt(
    const glm::ivec3& worldParticlePosition,
    const ParticleData::ParticleInfo& particleInfo
)
{
    const glm::ivec3 chunkPos = PositionConversion::ParticleGridToChunkGrid(worldParticlePosition, chunkParticleGridSize);
    if (chunkPos != chunkGridPosition) {
        return false;
    }

    // Block until able to acquire lock
    const std::lock_guard guard(lock);

    if (particleInfo.particleType == 0) {
        particleDataChunk->removeParticle(worldParticlePosition);
        // todo: might need to remove position for update queue??

        return false;
    }
    particleDataChunk->addParticle(worldParticlePosition, particleInfo);
    nextPositionsToUpdate.emplace_back(worldParticlePosition);

    // fixme: ??
    return false;
}

bool ParticlesChunk::SaveChunkData()
{
    // std::stringstream filenameForChunk;
    // filenameForChunk
    // << "chunks/"
    // << chunkGridPosition.x
    // << "-"
    // << chunkGridPosition.y
    // << "-"
    // << chunkGridPosition.z
    // << ".bin";
    //
    // std::vector<std::byte> bytes(sizeof(unsigned) * particleManager.GetParticleTypesData().size());
    // std::memcpy(bytes.data(), particleManager.GetParticleTypesData().data(), sizeof(unsigned) * particleManager.GetParticleTypesData().size());
    //
    // std::ofstream file(filenameForChunk.str(), std::ios::binary);
    // if (file) {
    //     file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    // }
    // file.close();

    return true;
}

bool ParticlesChunk::LoadChunkData()
{
    // fixme: temp
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
    //             reinterpret_cast<char*>( const_cast<unsigned*>(particleManager.GetParticleTypesData().data()) ),
    //             sizeof(unsigned) * 50 * 50 * 50
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

glm::vec3 ParticlesChunk::getChunkDistanceFrom(const glm::ivec3 &chunkPos) {
    return chunkPos - this->chunkGridPosition;
}

glm::ivec3 ParticlesChunk::getChunkWorldPosition() {
    return this->chunkGridPosition;
}
