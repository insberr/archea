//
// Created by insberr on 5/22/24.
//

#include <iostream>
#include "InputSystem.h"
#include "../App.h"
#include "ImGuiSystem.h"
#include "GraphicsSystem.h"

namespace InputSystem {
    /* System Function Declarations */
    // int Setup();
    void Init();
    void Update(float dt);
    void Render();
    void Exit();
    // void Done();
    System AsSystem() {
        return {
            nullptr,
            Init,
            Update,
            Render,
            Exit,
            nullptr
        };
    }

    /* Private Variables And Functions */


    // Keyboard
    std::unordered_map<int, bool> keysLastFrame;
    std::unordered_map<int, bool> keysThisFrame;

    // Mouse Buttons
    std::unordered_map<int, bool> mouseButtonLastFrame;
    std::unordered_map<int, bool> mouseButtonThisFrame;

    // Mouse Position
    std::pair<double, double> mousePositionLastFrame;
    std::pair<double, double> mousePositionThisFrame;

    // Mouse Scroll
    std::pair<double, double> mouseScrollLastFrame;
    std::pair<double, double> mouseScrollThisFrame;

    void keyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void mousePositionCallback(GLFWwindow* window, double xPosition, double yPosition);
    void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
}

void InputSystem::Init() {
    auto window = Graphics::GetWindow();
    glfwSetKeyCallback(window, keyboardInputCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePositionCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);

    // If raw mouse motion is supported, then enable it
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
}

void InputSystem::Update(float dt) {
}

void InputSystem::Render()
{
    keysLastFrame = keysThisFrame;
    mouseButtonLastFrame = mouseButtonThisFrame;
    mousePositionLastFrame = mousePositionThisFrame;

    mouseScrollLastFrame = mouseScrollThisFrame;
    mouseScrollThisFrame = { 0, 0 }; // todo: this cant be right...
}
void InputSystem::Exit() {}

bool InputSystem::IsKeyTriggered(int key) {
    auto stateLast = keysLastFrame.find(key);
    auto stateCurrent = keysThisFrame.find(key);

    bool last = stateLast != keysLastFrame.end() && stateLast->second;
    bool current = stateCurrent != keysThisFrame.end() && stateCurrent->second;

    return current && !last;
}

bool InputSystem::IsKeyHeld(int key) {
    auto state = keysThisFrame.find(key);
    return state != keysThisFrame.end() && state->second;
}

bool InputSystem::IsKeyReleased(int key) {
    auto stateLast = keysLastFrame.find(key);
    auto stateCurrent = keysThisFrame.find(key);

    bool last = stateLast != keysLastFrame.end() && stateLast->second;
    bool current = stateCurrent != keysThisFrame.end() && stateCurrent->second;

    return !current && last;
}

bool InputSystem::IsMouseButtonTriggered(int button)
{
    auto stateLast = mouseButtonLastFrame.find(button);
    auto stateCurrent = mouseButtonThisFrame.find(button);

    bool last = stateLast != mouseButtonLastFrame.end() && stateLast->second;
    bool current = stateCurrent != mouseButtonThisFrame.end() && stateCurrent->second;

    return current && !last;
}

bool InputSystem::IsMouseButtonHeld(int button)
{
    auto state = mouseButtonThisFrame.find(button);
    return state != mouseButtonThisFrame.end() && state->second;
}

bool InputSystem::IsMouseButtonReleased(int button)
{
    auto stateLast = mouseButtonLastFrame.find(button);
    auto stateCurrent = mouseButtonThisFrame.find(button);

    bool last = stateLast != mouseButtonLastFrame.end() && stateLast->second;
    bool current = stateCurrent != mouseButtonThisFrame.end() && stateCurrent->second;

    return !current && last;
}

std::pair<double, double> InputSystem::MousePosition() {
    return mousePositionThisFrame;
}

std::pair<double, double> InputSystem::MousePositionDelta() {
    return {
        mousePositionThisFrame.first - mousePositionLastFrame.first,
        mousePositionThisFrame.second - mousePositionLastFrame.second
    };
}

std::pair<double, double> InputSystem::MousePositionLast() {
    return mousePositionLastFrame;
}

std::pair<double, double> InputSystem::MouseScroll() {
    return mouseScrollThisFrame;
}

std::pair<double, double> InputSystem::MouseScrollDelta() {
    return {
        mouseScrollThisFrame.first - mouseScrollLastFrame.first,
        mouseScrollThisFrame.second - mouseScrollLastFrame.second
    };
}

std::pair<double, double> InputSystem::MouseScrollLast() {
    return mouseScrollLastFrame;
}

void InputSystem::keyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    if (action == GLFW_RELEASE) {
        keysThisFrame[key] = false;
    } if (action == GLFW_PRESS) {
        keysThisFrame[key] = true;
    }

}

void InputSystem::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (action == GLFW_RELEASE) {
        mouseButtonThisFrame[button] = false;
    } if (action == GLFW_PRESS) {
        mouseButtonThisFrame[button] = true;
    }

}

void InputSystem::mousePositionCallback(GLFWwindow *window, double xPosition, double yPosition) {
    ImGui_ImplGlfw_CursorPosCallback(window, xPosition, yPosition);
    if (ImGui::GetIO().WantCaptureMouse) return;

    mousePositionThisFrame = std::pair(xPosition, yPosition);
}

void InputSystem::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    if (ImGui::GetIO().WantCaptureMouse) return;

    mouseScrollThisFrame = std::pair(xoffset, yoffset);
}

