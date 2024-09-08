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
#include <bit>
#include <semaphore>

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
        return particlePosition / glm::ivec3(standardChunkParticleGridSize);
    }
}

ParticlesChunk::ParticlesChunk(
    const glm::ivec3& chunkGridPosition,
    const glm::uvec3& particleGridSize,
    float particleScale
) : particleScale(particleScale),
    worldPosition(),
    chunkGridPosition(chunkGridPosition),
    worldChunkScale(),
    chunkParticleGridSize(particleGridSize),
    particleManager(particleGridSize)
{
    worldChunkScale = glm::vec3(particleGridSize) * particleScale;
    worldPosition = glm::vec3(chunkGridPosition) * worldChunkScale;

    glCreateBuffers(1, &particlesBuffer);

    glNamedBufferStorage(
        particlesBuffer,
        sizeof(int) * particleManager.GetCubicSize(),
        static_cast<const void*>(particleManager.GetParticleTypesData().data()),
        GL_DYNAMIC_STORAGE_BIT
    );

    if (!LoadChunkData())
    {
        // todo: Temp, for testing
        for (unsigned x = 10; x < 20; ++x) {
            for (unsigned y = 5; y < 15; ++y) {
                for (unsigned z = 10; z < 20; ++z) {
                    particleManager.SetType(glm::uvec3(x, y, z), 2);
                }
            }
        }
    }
}

ParticlesChunk::~ParticlesChunk() {
    SaveChunkData();
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

    for (unsigned x = 0; x < chunkParticleGridSize.x; ++x) {
        for (int y = 0; y < chunkParticleGridSize.y; ++y) {
            for (unsigned z = 0; z < chunkParticleGridSize.z; ++z) {
                const auto currentPos = glm::ivec3(x, y, z);

                // z * (ysize * xsize) + y * (xsize) + x
                unsigned particle = particleManager.Get(currentPos).particleType;
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

                    unsigned atNewPos = particleManager.Get(posToTry).particleType;
                    if (atNewPos != 0) continue;

                    // We are here, we are allowed to move here

                    particleManager.SetType(posToTry, particle);
                    // remove
                    particleManager.SetType(glm::uvec3(x, y, z), 0);
                    break;
                    // if works, break
                    // else continue
                }
                //end
            }
        }
    }
}

void ParticlesChunk::Render(
    GLFWwindow* window,
    GLuint shaderProgram,
    GLuint particlesColorsBuffer,
    GLuint VAO
) {
    glNamedBufferSubData(
        particlesBuffer,
        0,
        sizeof(int) * particleManager.GetCubicSize(),
        static_cast<const void*>(particleManager.GetParticleTypesData().data())
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
    const ParticleData::DataWrapper& particleDataWraper
)
{
    const glm::ivec3 chunkPos = PositionConversion::ParticleGridToChunkGrid(worldParticlePosition, chunkParticleGridSize);
    if (chunkPos != chunkGridPosition) {
        return false;
    }

    // Block until able to aquire lock
    const std::lock_guard guard(lock);

    const glm::uvec3 particlePositionInChunk = glm::abs(worldParticlePosition - (chunkGridPosition * glm::ivec3(chunkParticleGridSize)));

    particleManager.SetType(particlePositionInChunk, particleDataWraper.particleType);

    return false;
}

bool ParticlesChunk::SaveChunkData()
{
    std::stringstream filenameForChunk;
    filenameForChunk
    << "chunks/"
    << chunkGridPosition.x
    << "-"
    << chunkGridPosition.y
    << "-"
    << chunkGridPosition.z
    << ".bin";

    std::vector<std::byte> bytes(sizeof(unsigned) * particleManager.GetParticleTypesData().size());
    std::memcpy(bytes.data(), particleManager.GetParticleTypesData().data(), sizeof(unsigned) * particleManager.GetParticleTypesData().size());

    std::ofstream file(filenameForChunk.str(), std::ios::binary);
    if (file) {
        file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    }
    file.close();

    return true;
}

bool ParticlesChunk::LoadChunkData()
{
    try {
        std::stringstream filenameForChunk;
        filenameForChunk
        << "chunks/"
        << chunkGridPosition.x
        << "-"
        << chunkGridPosition.y
        << "-"
        << chunkGridPosition.z
        << ".bin";

        std::ifstream file(filenameForChunk.str(), std::ios::binary);

        if (file) {
            file.read(
                reinterpret_cast<char*>( const_cast<unsigned*>(particleManager.GetParticleTypesData().data()) ),
                sizeof(unsigned) * 50 * 50 * 50
            );
        } else {
            file.close();
            return false;
        }
        file.close();

        return true;
    } catch (std::exception& e) {
        return false;
    }
}
