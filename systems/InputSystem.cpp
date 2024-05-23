//
// Created by insberr on 5/22/24.
//

#include "InputSystem.h"
#include "../App.h"

// Initialize the static variables
// Keyboard
std::unordered_map<int, bool> InputSystem::keysLastFrame;
std::unordered_map<int, bool> InputSystem::keysThisFrame;
// Mouse Buttons
std::unordered_map<int, bool> InputSystem::mouseButtonThisFrame;
// Mouse Position
std::pair<double, double> InputSystem::mousePositionLastFrame;
std::pair<double, double> InputSystem::mousePositionThisFrame;

InputSystem::~InputSystem() = default;

void InputSystem::Init() {
    auto window = app->GetWindow();
    glfwSetKeyCallback(window, keyboardInputCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePositionCallback);

    // If raw mouse motion is supported, then enable it
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
}

void InputSystem::Update(float dt) {
    keysLastFrame = keysThisFrame;
    mousePositionLastFrame = mousePositionThisFrame;
}

void InputSystem::Render() {}
void InputSystem::Exit() {}

bool InputSystem::IsKeyTriggered(int key) {
    auto stateLast = keysLastFrame.find(key);
    auto stateCurrent = keysThisFrame.find(key);

    bool last = stateLast != keysLastFrame.end() && !stateLast->second;
    bool current = stateLast != keysThisFrame.end() && !stateCurrent->second;

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
    bool current = stateLast != keysThisFrame.end() && stateCurrent->second;

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

void InputSystem::keyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) {
        keysThisFrame[key] = false;
    }
    if (action == GLFW_PRESS) {
        keysThisFrame[key] = true;
    }

}

void InputSystem::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {

}

void InputSystem::mousePositionCallback(GLFWwindow *window, double xPosition, double yPosition) {
    mousePositionThisFrame = std::pair(xPosition, yPosition);
}
