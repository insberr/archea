//
// Created by insberr on 5/22/24.
//

#include <glm/gtc/quaternion.hpp>
#include "../App.h"
#include "CameraSystem.h"

#include <glm/gtc/type_ptr.hpp>

#include "InputSystem.h"
#include "ImGuiSystem.h"
#include "GraphicsSystem.h"
#include "Player.h"
#include "Shapes.h"
#include "../shaders.h"

namespace CameraSystem {
    /* System Function Declarations */
    // int Setup();
    void Init();
    // void Update(float dt);
    void Render();
    // void Exit();
    // void Done();
    System AsSystem() {
        return {
                nullptr, // Setup,
                Init,
                nullptr,
                Render,
                nullptr,
                nullptr, // Done
        };
    }

    /* Private Variables And Functions */

    const glm::vec3 UpVector(0.0f, 1.0f, 0.0f);

    void updateViewMatrix();
    void updateProjectionMatrix();

    // Translate the camera some amount added to the current position
    void Translate(glm::vec3 translation);

    glm::vec3 position;
    glm::vec3 target;
    float fieldOfView { 0.7f };

    glm::mat4 view;
    glm::mat4 projection;
}

void CameraSystem::Init() {
    view = glm::mat4(1.0);
    projection = glm::mat4(1.0);
    Reset();
}

void CameraSystem::Render() {
    if (ImGui::Begin("Camera")) {
        ImGui::Text("Position %.2f %.2f %.2f", position.x, position.y, position.z);
        ImGui::Text("Look Direction %.2f %.2f %.2f", target.x, target.y, target.z);

        if (ImGui::SliderFloat("Field Of View", &fieldOfView, 0.0f, 90.0f, "%.4f")) {
            updateProjectionMatrix();
        }

        if (ImGui::Button("Reset")) {
            Reset();
        }
    }
    ImGui::End();
}

glm::vec3 CameraSystem::GetPosition() {
    return position;
}

glm::vec3 CameraSystem::GetTarget() {
    return target;
}

void CameraSystem::Reset() {
    position = glm::vec3(0);
    target = glm::vec3(0);
    fieldOfView = 60.0f;

    updateViewMatrix();
    updateProjectionMatrix();
}

void CameraSystem::updateViewMatrix() {
    view = glm::lookAt(position, position + target, UpVector);
}

void CameraSystem::updateProjectionMatrix() {
    // TODO: Use window size dynamically
    float aspectRatio = 1280.0f / 720.0f;
    projection = glm::perspective(glm::radians(fieldOfView), aspectRatio, 0.1f, 100.0f);
}

void CameraSystem::set(const glm::vec3& newPosition, const glm::vec3& newTarget) {
    position = newPosition;
    target = newTarget;

    updateViewMatrix();
}

glm::mat4 CameraSystem::CameraMatrix() {
    return projection * view;
}

float CameraSystem::GetFOV() {
    return fieldOfView;
}

glm::mat4 CameraSystem::GetView() {
    return view;
}

glm::mat4 CameraSystem::GetProjection() {
    return projection;
}
