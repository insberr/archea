//
// Created by insberr on 7/12/25.
//

#pragma once
#include <functional>
#include <string>
#include <glm/vec2.hpp>

#include "../System.h"

namespace GUI {
    System AsSystem();

    struct Element {
        int id;
        glm::vec2 position;
        glm::vec2 size;
        std::string title;
        bool isHovered;
        bool isPressed;
        bool isReleased;
        bool isTriggered;
    };

    Element GetElementById(const int id);

    int CreateButton(
        const glm::vec2& position,
        const glm::vec2& size,
        const std::string& text
    );
};
