//
// Created by insberr on 5/22/24.
//

#pragma once
#include "System.h"
#include <unordered_map>

typedef struct GLFWwindow GLFWwindow;

class InputSystem : public System {
public:
    InputSystem() : System("InputSystem") {};
    ~InputSystem() override;

    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Exit() override;


    static bool IsKeyTriggered(int key);
    static bool IsKeyHeld(int key);
    static bool IsKeyReleased(int key);

    static std::pair<double, double> MousePosition();
    static std::pair<double, double> MousePositionDelta();
    static std::pair<double, double> MousePositionLast();

private:
    // Keyboard
    static std::unordered_map<int, bool> keysLastFrame;
    static std::unordered_map<int, bool> keysThisFrame;

    // Mouse Buttons
    static std::unordered_map<int, bool> mouseButtonThisFrame;

    // Mouse Position
    static std::pair<double, double> mousePositionLastFrame;
    static std::pair<double, double> mousePositionThisFrame;

    static void keyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mousePositionCallback(GLFWwindow* window, double xPosition, double yPosition);
};
