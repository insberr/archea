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
        bool disabled;
        bool isHovered;
        bool isPressed;
        bool isReleased;
        bool isTriggered;
    };

    Element GetElementById(const int id);
    void RemoveElement(const int id);
    void DisableElement(const int id);
    void EnableElement(const int id);

    void UpdateElementPosition(int id, const glm::vec2& position);
    void UpdateElementSize(int id, const glm::vec2& size);

    int CreateButton(
        const glm::vec2& position,
        const glm::vec2& size,
        const std::string& text
    );
};
