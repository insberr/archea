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
#include "Shapes.h"
#include "../shaders.h"

namespace CameraSystem {
    /* System Function Declarations */
    // int Setup();
    void Init();
    void Update(float dt);
    void Render();
    void Exit();
    // void Done();
    System AsSystem() {
        return {
                nullptr, // Setup,
                Init,
                Update,
                Render,
                Exit,
                nullptr, // Done
        };
    }

    /* Private Variables And Functions */

    const glm::vec3 UpVector(0.0f, 1.0f, 0.0f);

    // Translate the camera some amount added to the current position
    void Translate(glm::vec3 translation);

    glm::vec3 position;
    glm::vec3 target;
    float fieldOfView { 0.7f };

    /* New */
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    float yaw { -90.0f };
    float pitch { 0.0f };
    float sensitivity { 0.3f };
    float travelSpeed { 5.0f };

    bool cursorLocked { false };

    GLuint shaderProgram { 0 };
    GLuint VBO {0}, VAO {0}, EBO {0};
}

void CameraSystem::Init() {
    model = glm::mat4(1.0);
    view = glm::mat4(1.0);
    projection = glm::mat4(1.0);

    Reset();


    // Load the contents of the shaders
    std::string vertexShaderSource = readShaderFile("shaders/vertex_normal_cube.glsl");
    std::string fragmentShaderSource = readShaderFile("shaders/fragment_normal_cube.glsl");

    // Make sure they arent empty
    if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
        return;
    }

    // Create the shader program
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void CameraSystem::Update(float dt) {
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

    if (InputSystem::IsKeyHeld(GLFW_KEY_W)) {
        Translate(glm::vec3(0.0f, 0.0f, -dt));
    }
    if (InputSystem::IsKeyHeld(GLFW_KEY_S)) {
        Translate(glm::vec3(0.0f, 0.0f, dt));
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
        travelSpeed = 40.0f;
    } else {
        travelSpeed = 20.0f;
    }

    // Dont update look position
    if (cursorLocked) {

        auto [mouseX, mouseY] = InputSystem::MousePosition();
        auto [lastX, lastY] = InputSystem::MousePositionLast();

        float xOffset = static_cast<float>(mouseX - lastX);
        float yOffset = static_cast<float>(lastY - mouseY);

        yaw += xOffset * sensitivity;
        pitch += yOffset * sensitivity;

        // Clamp pitch to avoid flipping
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
    }

    // Calculate the front vector
    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    target = glm::normalize(front);

    // View
    view = glm::lookAt(position, position + target, UpVector);

    // Projection
    // TODO: Use window size dynamically
    float aspectRatio = 1280.0f / 720.0f;
    projection = glm::perspective(glm::radians(fieldOfView), aspectRatio, 0.1f, 100.0f);
}

void CameraSystem::Render() {
    if (ImGui::Begin("Camera")) {
        ImGui::Text("Position %.2f %.2f %.2f", position.x, position.y, position.z);
        ImGui::Text("Look Direction %.2f %.2f %.2f", target.x, target.y, target.z);
        ImGui::Text("Pitch Yaw %.2f %.2f", pitch, yaw);

        ImGui::SliderFloat("Field Of View", &fieldOfView, 0.0f, 90.0f, "%.4f");
        ImGui::SliderFloat("Sensitivity %.2f", &sensitivity, 0.0001f, 1.0f, "%.4f");
        ImGui::SliderFloat("Movement Speed", &travelSpeed, 0.0f, 50.0f, "%.4f");

        if (ImGui::Button("Reset")) {
            Reset();
        }
    }
    ImGui::End();

    // Temp, Render a small cube where I am looking
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

    // TEMP ,, do render
    auto window = Graphics::GetWindow();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    auto model = glm::scale(
        glm::translate(
            GetModel(),
            position + target
        ),
        glm::vec3(0.03f)
    );
    auto view = GetView();
    auto projection = GetProjection();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Bind the vertex data
    glBindVertexArray(VAO);
    // Call draw
    glDrawElements(GL_TRIANGLES, sizeof(Shapes::Cube::indices), GL_UNSIGNED_INT, 0);
}

void CameraSystem::Exit() {}

glm::vec3 CameraSystem::GetPosition() {
    return position;
}

glm::vec3 CameraSystem::GetTarget() {
    return target;
}

void CameraSystem::Reset() {
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    // target = glm::vec3(0.0f, 0.0f, 5.0f);
    fieldOfView = 60.0f;
    yaw = -90.0f;
    pitch = 0.0f;
    sensitivity = 0.3f;
    travelSpeed = 20.0f;
}

glm::mat4 CameraSystem::CameraMatrix() {

    return glm::mat4(glm::quatLookAtLH(target, UpVector));
}

void CameraSystem::Translate(glm::vec3 translation) {
    // Separate vertical movement so it is not affected by rotation
    glm::vec3 vertical(0.0f, translation.y, 0.0f);

    // Compute the horizontal forward vector from the current target (ignoring y).
    glm::vec3 forward = glm::normalize(glm::vec3(target.x, 0.0f, target.z));

    // Calculate the right vector using the world up.
    glm::vec3 right = glm::normalize(glm::cross(forward, UpVector));

    // To map this correctly, invert the z value when combining:
    glm::vec3 horizontal = translation.x * right - translation.z * forward;

    position += travelSpeed * (horizontal + vertical);
}

float CameraSystem::GetFOV() {
    return fieldOfView;
}

glm::mat4 CameraSystem::GetModel() {
    return model;
}

glm::mat4 CameraSystem::GetView() {
    return view;
}

glm::mat4 CameraSystem::GetProjection() {
    return projection;
}
