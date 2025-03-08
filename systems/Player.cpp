//
// Created by jonah on 2/28/2025.
//

#include "Player.h"

#include <algorithm>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "CameraSystem.h"
#include "GraphicsSystem.h"
#include "ImGuiSystem.h"
#include "InputSystem.h"
#include "ParticleSystem.h"
#include "particle_types/ParticleTypeSystem.h"
#include "particle_types/ParticleType.h"
#include "../shaders.h"
#include "Shapes.h"

int div_euclid(float a, float b) {

    int q = static_cast<int>(a / b); //.trunc();
    if (fmod(a, b) < 0.0f) {
        if (b > 0.0f) {
            return q - 1;
        } else {
            return q + 1;
        };
    }
    return q;
}

Player::Player() :
position(glm::vec3(0.0f, 0.0f, 0.0f)),
front(glm::vec3(0)),
velocity(glm::vec3(0.0f, 0.0f, 0.0f)),
yaw(-90.0f),
pitch(0.0f),
mode(Player::Mode::Gravity)
{
}

void Player::initGraphics() {
    // Load the contents of the shaders
    std::string vertexShaderSource = readShaderFile("shaders/vertex_normal_cube.glsl");
    std::string fragmentShaderSource = readShaderFile("shaders/fragment_normal_cube.glsl");

    // Make sure they arent empty
    if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
        return;
    }

    // Create the shader program
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // // TODO: Reuse this buffer (from ParticleSystem) rather than making another
    // glCreateBuffers(1, &particlesColorsBuffer);
    // auto particleColors = ParticleTypeSystem::GetParticleColorIndexesForShader();
    // glNamedBufferStorage(
    //     particlesColorsBuffer,
    //     sizeof(NormColor) * particleColors.size(),
    //     (const void*)particleColors.data(),
    //     GL_DYNAMIC_STORAGE_BIT
    // );
    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, particlesColorsBuffer);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void Player::update(float dt, const std::vector<ParticlesChunk*>& particleChunks) {
    if (dt == 0.0f) return;

    // Update physics
    updatePhysics(dt);

    // Handle collisions
    // TODO

    // Update camera position
    CameraSystem::set(position, front);

    // Handle placing particles
    handleParticlePlacing(dt, particleChunks);
}

void Player::render() {
    drawGhostCube();

    if (ImGui::Begin("Player")) {
        ImGui::Text("Position %.2f %.2f %.2f", position.x, position.y, position.z);
        ImGui::Text("Look Direction %.2f %.2f %.2f", front.x, front.y, front.z);
        ImGui::Text("Velocity %.2f %.2f %.2f", front.x, front.y, front.z);
        ImGui::Text("Pitch Yaw %.2f %.2f", pitch, yaw);

        ImGui::SliderFloat("Mouse Sensitivity %.2f", &rotationSensitivity, 0.0001f, 1.0f, "%.4f");
        ImGui::SliderFloat("Movement Speed", &movementSpeed, 0.0f, 50.0f, "%.4f");

        if (ImGui::Button("Reset")) {
            yaw = -90.0f;
            pitch = 0.0f;
            position = glm::vec3(0.0f);
        }
    }
    ImGui::End();

    if (ImGui::Begin("Drawing Controls"))
    {
        ImGui::Text("Drawing is based on where you are looking");
        ImGui::Text("Hold Left Mouse Button to draw");
        ImGui::Text("Hold Right Mouse Button to erase");

        ImGui::Text("Drawing Position %i %i %i", lookingAtParticlePos.x, lookingAtParticlePos.y, lookingAtParticlePos.z);
        ImGui::SliderInt("Draw Type", reinterpret_cast<int *>(&drawType), 1, ParticleTypeSystem::GetParticleTypeCount(), ParticleTypeSystem::GetParticleTypeInfo(drawType - 1).nameId);
    }
    ImGui::End();
}

glm::vec3 Player::getPosition() {
    return position;
}

void Player::updatePhysics(float dt) {
    auto window = Graphics::GetWindow();

    if (InputSystem::IsKeyTriggered(GLFW_KEY_ESCAPE)) {
        if (cursorLocked) {
            // Unlock the cursor and show it
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            ImGuiSystem::EnableImGui();
        } else {
            // Lock the cursor and hide it
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            ImGuiSystem::DisableImGui();
        }
        cursorLocked = !cursorLocked;
    }

    // Dont update look position
    if (cursorLocked) {
        // TODO: Does the camera movement need to be multiplied by dt?
        auto [mouseX, mouseY] = InputSystem::MousePosition();
        auto [lastX, lastY] = InputSystem::MousePositionLast();

        float xOffset = static_cast<float>(mouseX - lastX);
        float yOffset = static_cast<float>(lastY - mouseY);

        yaw += xOffset * rotationSensitivity;
        pitch += yOffset * rotationSensitivity;

        // Clamp pitch to avoid flipping
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
    }

    // Calculate the front vector
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    front = glm::normalize(front);


    glm::vec3 acceleration(0);

    if (InputSystem::IsKeyHeld(GLFW_KEY_W)) {
        acceleration = glm::vec3(0, 0, -dt);
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_S)) {
        acceleration = glm::vec3(0.0f, 0.0f, dt);
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_A)) {
        acceleration = glm::vec3(-dt, 0.0f, 0.0f);
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_D)) {
        acceleration = glm::vec3(dt, 0.0f, 0.0f);
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_SPACE)) {
        acceleration = glm::vec3(0.0f, dt, 0.0f);
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_LEFT_SHIFT)) {
        acceleration = glm::vec3(0.0f, -dt, 0.0f);
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_LEFT_CONTROL)) {
        movementSpeed = 40.0f;
    } else {
        movementSpeed = 20.0f;
    }

    // Separate vertical movement so it is not affected by rotation
    glm::vec3 vertical(0.0f, acceleration.y, 0.0f);
    // Compute the horizontal forward vector from the current target (ignoring y).
    glm::vec3 forward = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    // Calculate the right vector using the world up.
    glm::vec3 right = glm::normalize(glm::cross(forward, UpVector));
    // To map this correctly, invert the z value when combining:
    glm::vec3 horizontal = acceleration.x * right - acceleration.z * forward;
    acceleration = horizontal + vertical;

    position += acceleration * movementSpeed;
}

void Player::handleParticlePlacing(float dt, const std::vector<ParticlesChunk*>& particleChunks) {
    std::pair<double, double> mouseScroll = InputSystem::MouseScroll();

    drawDistance += static_cast<float>(mouseScroll.second);
    drawDistance = std::clamp(drawDistance, 1.0f, 100.0f);

    const glm::vec3 camPos = CameraSystem::GetPosition();
    const glm::vec3 camTarget = CameraSystem::GetTarget();

    const glm::vec3 lookingAt = camPos + (camTarget * drawDistance);

    // todo: it might be good to store the converted particle coordinate camPos and camTarget positions
    // lookingAtParticlePos = lookingAt / particleScale;
    lookingAtParticlePos.x = div_euclid(lookingAt.x, ParticleSystem::particleScale);
    lookingAtParticlePos.y = div_euclid(lookingAt.y, ParticleSystem::particleScale);
    lookingAtParticlePos.z = div_euclid(lookingAt.z, ParticleSystem::particleScale);

    if (InputSystem::IsMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
        // particleDataManager.SetType(drawPos, drawType);
        // todo: this is not really efficient
        for (const auto& chunk : particleChunks) {
            chunk->tryPlaceParticleAt(
                lookingAtParticlePos,
                { drawType, 0.0f }
            );
        }
    }

    if (InputSystem::IsMouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT)) {
        // particleDataManager.SetType(drawPos, 0);
        for (const auto& chunk : particleChunks) {
            chunk->tryPlaceParticleAt(
                lookingAtParticlePos,
                { 0, 0.0f }
            );
        }
    }
}

void Player::drawGhostCube() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes::Cube::cubeVertices), Shapes::Cube::cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Shapes::Cube::indices), &Shapes::Cube::indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // todo: throw error if no shader program???
    if (shaderProgram == 0) return;
    glUseProgram(shaderProgram);

    GLuint particleTypeLoc = glGetUniformLocation(shaderProgram, "particleType");
    GLuint positionLoc = glGetUniformLocation(shaderProgram, "position");
    GLuint particleScaleLoc = glGetUniformLocation(shaderProgram, "particleScale");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    auto view = CameraSystem::GetView();
    auto projection = CameraSystem::GetProjection();
    glUniform1ui(particleTypeLoc, drawType - 1);
    glUniform1f(particleScaleLoc, ParticleSystem::particleScale);
    glUniform3f(positionLoc, lookingAtParticlePos.x, lookingAtParticlePos.y, lookingAtParticlePos.z);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Bind the vertex data
    glBindVertexArray(VAO);
    // Call draw
    glDrawElements(GL_TRIANGLES, sizeof(Shapes::Cube::indices), GL_UNSIGNED_INT, 0);
}
