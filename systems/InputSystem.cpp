//
// Created by insberr on 5/22/24.
//

#include "InputSystem.h"
#include "../App.h"

// Initialize the static variables
std::unordered_map<int, bool> InputSystem::keysLastFrame;
std::unordered_map<int, bool> InputSystem::keysThisFrame;

InputSystem::~InputSystem() = default;

void InputSystem::Init() {
    auto window = app->GetWindow();
    glfwSetKeyCallback(window, keyboardInputCallback);
}

void InputSystem::Update(float dt) {
    keysLastFrame = keysThisFrame;
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


void InputSystem::keyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) {
        keysThisFrame[key] = false;
    }
    if (action == GLFW_PRESS){
        keysThisFrame[key] = true;
    }

}
