//
// Created by jonah on 2/28/2025.
//

#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <GL/glew.h>

// TODO: Todo move this somewhere else
#include <cmath>
#include <vector>

#include "ParticlesChunk.h"

class Player {
public:
    enum class Mode {
        Gravity,
        Fly
    };

    Player();

    void initGraphics();

    void update(float dt, const std::unordered_map<glm::ivec3, ParticlesChunk*>& particleChunks);
    void render();

    glm::vec3 getPosition();

private:
    void updatePhysics(float dt);
    void handleParticlePlacing(float dt, const std::unordered_map<glm::ivec3, ParticlesChunk*>& particleChunks);
    void drawGhostCube() const;

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 velocity;

    float yaw = -90.0f;
    float pitch = 0.0f;

    Mode mode;


    float movementSpeed = 20.0f;
    float rotationSensitivity = 0.3f;
    glm::vec3 Gravity { 0.0f, -9.8f, 0.0f };

    const glm::vec3 UpVector { 0.0f, 1.0f, 0.0f };
    bool cursorLocked = false;

    /* Particle Placing */
    glm::ivec3 lookingAtParticlePos;
    float drawDistance { 5.0f };
    unsigned drawType = 2;

    /* Ghost Cube Rendering */
    GLuint shaderProgram { 0 };
    GLuint VBO {0}, VAO {0}, EBO {0};
    // GLuint particlesColorsBuffer { 0 };
};
