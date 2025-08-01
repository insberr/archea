//
// Created by insberr on 7/12/25.
//

#include "GUI.h"

#include <functional>
#include <ranges>
#include <unordered_map>
#include <glm/vec2.hpp>

#include "imgui.h"
#include "../GraphicsSystem.h"
#include "../InputSystem.h"

namespace GUI {
    void Update(float dt);
    void PostRender();
    void Done();
    System AsSystem() {
        return {
            .Update = Update,
            // .Render = Render,
            .PostRender = PostRender,
            .Done = Done,
        };
    }

    std::unordered_map<int, Element> Elements;
    int lastID = 0;
}

void GUI::Update(float dt) {
    for (auto& element : Elements | std::ranges::views::values) {
        if (element.disabled) continue;

        const Element lastFrameElement = element;

        // Update hovered, pressed, and released
        // Hovered: Is mouse inside bounding box?
        const glm::ivec2 windowSize = Graphics::GetWindowSize();
        if (const auto [mouseX, mouseY] = InputSystem::MousePosition();
            (mouseX > element.position.x - (element.size.x / 2)) &&
            (mouseX < element.position.x + (element.size.x / 2)) &&
            // For some reason drawing, up is positive y, but mouse down is positive y
            (windowSize.y - mouseY > element.position.y - (element.size.y / 2)) &&
            (windowSize.y - mouseY < element.position.y + (element.size.y / 2))
        ) {
            element.isHovered = true;
        } else {
            element.isHovered = false;
        }
        // Pressed: Is hovered and is mouse button down?
        if (element.isHovered && InputSystem::IsMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
            element.isPressed = true;
        } else {
            element.isPressed = false;
        }
        // Released: if was pressed last frame and is no longer pressed
        if (lastFrameElement.isPressed && InputSystem::IsMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT)) {
            element.isReleased = true;
        } else {
            element.isReleased = false;
        }
        // Triggered: Is released and is hovered
        if (element.isReleased && element.isHovered) {
            element.isTriggered = true;
        } else {
            element.isTriggered = false;
        }
    }
}

void GUI::PostRender() {
    for (const auto& element : Elements | std::ranges::views::values) {
        if (element.disabled) continue;
        // Render a rect at position with size. using opengl
        glDisable(GL_DEPTH_TEST);
        // Graphics::Draw2D::DrawRectangle(element.position, element.size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        glm::vec3 color = glm::vec3(1.0f);
        if (element.isHovered) {
            color = glm::vec3(0.5f);
        }
        Graphics::Draw2D::DrawSprite(
            "./sprites/button.png",
            element.position - (element.size / 2.0f),
            element.size,
            0.0f,
            color
        );
        Graphics::Draw2D::DrawText(element.title, element.position, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glEnable(GL_DEPTH_TEST);
    }
}

void GUI::Done() {

}

GUI::Element GUI::GetElementById(const int id) {
    return GUI::Elements.at(id);
}

void GUI::RemoveElement(const int id) {
    Elements.erase(id);
}

void GUI::DisableElement(const int id) {
    Elements.at(id).disabled = true;
}

void GUI::EnableElement(const int id) {
    Elements.at(id).disabled = false;
}

void GUI::UpdateElementPosition(int id, const glm::vec2 &position) {
    Elements.at(id).position = position;
}

void GUI::UpdateElementSize(int id, const glm::vec2 &size) {
    Elements.at(id).size = size;
}

int GUI::CreateButton(
    const glm::vec2& position,
    const glm::vec2& size,
    const std::string& text
) {
    int id = ++lastID;

    Element element = {
        id,
        position,
        size,
        text,
        false,
        false,
        false,
        false
    };

    Elements.insert({ id, element });

    return id;
}
