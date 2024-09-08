//
// Created by insberr on 5/22/24.
//

#pragma once
#include "System.h"
#include <unordered_map>

typedef struct GLFWwindow GLFWwindow;

namespace InputSystem {
    System AsSystem();

    /* Implementation Here */

    bool IsKeyTriggered(int key);
    bool IsKeyHeld(int key);
    bool IsKeyReleased(int key);

    bool IsMouseButtonTriggered(int button);
    bool IsMouseButtonHeld(int button);
    bool IsMouseButtonReleased(int button);

    std::pair<double, double> MousePosition();
    std::pair<double, double> MousePositionDelta();
    std::pair<double, double> MousePositionLast();

    std::pair<double, double> MouseScroll();
    std::pair<double, double> MouseScrollDelta();
    std::pair<double, double> MouseScrollLast();
};
