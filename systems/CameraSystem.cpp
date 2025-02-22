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
    std::string vertexShaderSource = readShaderFile("shaders/vertex_old3.glsl");
    std::string fragmentShaderSource = readShaderFile("shaders/plain_color.glsl");

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
    if (cursorLocked) {

        auto [mouseX, mouseY] = InputSystem::MousePosition();
        auto [lastX, lastY] = InputSystem::MousePositionLast();

        float xoffset = static_cast<float>(mouseX - lastX);
        float yoffset = static_cast<float>(mouseY - lastY);

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Clamp pitch to avoid flipping
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }

    // Forward base vector
    auto forwardBaseVector = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw rotation around Y-axis
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch rotation around X-axis
    target = rotationMatrix * glm::vec4(forwardBaseVector, 0.0f);


    /* Calculate Camera Matrices */
    // Model
    // glm::translate(model, position);

    // View
    view = glm::lookAt(position, position + target, UpVector);
    // view = glm::mat4(glm::quatLookAtLH(target, UpVector));
    // Flip the y and z axes
    glm::mat4 flipX = glm::scale(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 1.0f));
    view = flipX * view;

    // Projection
    float aspectRatio = 1280.0f / 720.0f;
    projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.001f, 100.0f);
}

constexpr float cubeVertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,  // 0
    0.5f, -0.5f,  0.5f,  // 1
    0.5f,  0.5f,  0.5f,  // 2
    -0.5f,  0.5f,  0.5f,  // 3

    // Back face
    -0.5f, -0.5f, -0.5f,  // 4
    0.5f, -0.5f, -0.5f,  // 5
    0.5f,  0.5f, -0.5f,  // 6
    -0.5f,  0.5f, -0.5f,  // 7

    // Top face
    0.5f,  0.5f,  0.5f,  // 8
    -0.5f,  0.5f,  0.5f,  // 9
    -0.5f,  0.5f, -0.5f,  // 10
    0.5f,  0.5f, -0.5f,  // 11

    // Bottom face
    -0.5f, -0.5f,  0.5f,  // 12
    0.5f, -0.5f,  0.5f,  // 13
    0.5f, -0.5f, -0.5f,  // 14
    -0.5f, -0.5f, -0.5f,  // 15

    // Right face
    0.5f, -0.5f,  0.5f,  // 16
    0.5f, -0.5f, -0.5f,  // 17
    0.5f,  0.5f, -0.5f,  // 18
    0.5f,  0.5f,  0.5f,  // 19

    // Left face
    -0.5f, -0.5f,  0.5f,  // 20
    -0.5f, -0.5f, -0.5f,  // 21
    -0.5f,  0.5f, -0.5f,  // 22
    -0.5f,  0.5f,  0.5f   // 23
};

// Define the indices for the cube
constexpr GLuint indices[] = {
    // Front face
    0, 1, 2,
    2, 3, 0,

    // Back face
    4, 5, 6,
    6, 7, 4,

    // Top face
    8, 9, 10,
    10, 11, 8,

    // Bottom face
    12, 13, 14,
    14, 15, 12,

    // Right face
    16, 17, 18,
    18, 19, 16,

    // Left face
    20, 21, 22,
    22, 23, 20
};

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

    // Temp, Render a small cube where I am looking
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
        glm::vec3(0.05f)
    );
    auto view = GetView();
    auto projection = GetProjection();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(shaderProgram, "Color"), 0.7f, 0.3f, 0.4f);

    // Bind the vertex data
    glBindVertexArray(VAO);
    // Call draw
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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
