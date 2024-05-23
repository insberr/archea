//
// Created by insberr on 5/22/24.
//

#include <glm/gtc/quaternion.hpp>
#include "../App.h"
#include "CameraSystem.h"
#include "InputSystem.h"
#include "ImGuiSystem.h"

const glm::vec3 UpVector(0.0f, 1.0f, 0.0f);

CameraSystem::~CameraSystem() = default;

void CameraSystem::Init() {
    Reset();
}

void CameraSystem::Update(float dt) {
    auto window = app->GetWindow();
    if (InputSystem::IsKeyTriggered(GLFW_KEY_ESCAPE)) {
        if (cursorLocked) {
            // Unlock the cursor and show it
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            // Lock the cursor and hide it
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        cursorLocked = !cursorLocked;
    }

    if (InputSystem::IsKeyHeld(GLFW_KEY_W)) {
        Translate(glm::vec3(0.0f, 0.0f, dt));
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_S)) {
        Translate(glm::vec3(0.0f, 0.0f, -dt));
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_A)) {
        Translate(glm::vec3(-dt, 0.0f, 0.0f));
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_D)) {
        Translate(glm::vec3(dt, 0.0f, 0.0f));
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_SPACE)) {
        Translate(glm::vec3(0.0f, dt, 0.0f));
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_LEFT_SHIFT)) {
        Translate(glm::vec3(0.0f, -dt, 0.0f));
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_LEFT_CONTROL)) {
        travelSpeed = 10.0f;
    }

    // Dont update look position
    if (!cursorLocked) return;

    auto [mouseX, mouseY] = InputSystem::MousePosition();
    auto [lastX, lastY] = InputSystem::MousePositionLast();

    float xoffset = static_cast<float>(mouseX - lastX);
    float yoffset = static_cast<float>(mouseY - lastY); // Reversed since y-coordinates go from bottom to top

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Clamp pitch to avoid flipping
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Forward base vector
    auto forwardBaseVector = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw rotation around Y-axis
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch rotation around X-axis
    target = rotationMatrix * glm::vec4(forwardBaseVector, 0.0f);
}

void CameraSystem::Render() {
    if (ImGui::Begin("Camera")) {
        ImGui::Text("Position %.2f %.2f %.2f", position.x, position.y, position.z);
        ImGui::Text("Look Direction %.2f %.2f %.2f", target.x, target.y, target.z);
        ImGui::Text("Pitch Yaw %.2f %.2f", pitch, yaw);
        ImGui::SliderFloat("Field Of View", &fieldOfView, 0.0f, 5.0f, "%.4f");
        ImGui::SliderFloat("Movement Speed", &travelSpeed, 0.0f, 20.0f, "%.4f");

        if (ImGui::Button("Reset")) {
            Reset();
        }
    }
    ImGui::End();
}

void CameraSystem::Exit() {}

glm::vec3 CameraSystem::GetPosition() {
    return position;
}

glm::vec3 CameraSystem::GetTarget() {
    return target;
}

void CameraSystem::Reset() {
    position = glm::vec3(4.0f, 4.0f, -10.0f);
    target = glm::vec3(0.2f, -0.11f, 0.97f);
    fieldOfView = 1.0f;
    yaw = 11.40f;
    pitch = 6.30f;
    sensitivity = 0.3f;
    travelSpeed = 5.0f;
}

glm::mat4 CameraSystem::CameraMatrix() {

    return glm::mat4(glm::quatLookAtLH(target, UpVector));
}

void CameraSystem::Translate(glm::vec3 translation) {
    // Roll pitch yaw matrix
    // Yaw rotation around Y-axis
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    // Pitch rotation around X-axis
    // Don't do this because I want vertical movement to be controlled by shift and space
    // rotationMatrix = glm::rotate(rotationMatrix, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));

    auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(travelSpeed));

    auto transform = (rotationMatrix * scale) * glm::vec4(translation, 0.0f);

    position += glm::vec3(transform);
}

float CameraSystem::GetFOV() const {
    return fieldOfView;
}
