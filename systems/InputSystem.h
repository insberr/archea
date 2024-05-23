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


    static bool IsKeyTriggered(int key) ;
    static bool IsKeyHeld(int key) ;
    static bool IsKeyReleased(int key) ;

private:
    static std::unordered_map<int, int> keysLastFrame;
    static std::unordered_map<int, int> keysThisFrame;

    static void keyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
