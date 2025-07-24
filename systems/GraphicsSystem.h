//
// Created by insberr on 5/27/24.
//

#pragma once
#include "System.h"

// Include OpenGL Stuff
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Graphics {
    System AsSystem();

    /* Implementation Here */

    // Some constants
    const unsigned WindowWidth = 1920;
    const unsigned WindowHeight = 1080;

    // Get the window value created by glfw
    GLFWwindow* GetWindow();
    glm::ivec2 GetWindowSize();

    bool DidWindowResize();

    // Generate a shader program
    GLuint CreateShaderProgram();

    namespace Draw2D {
        // void DrawLine(int x1, int y1, int x2, int y2, int color);
        void DrawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        // void DrawCircle(int x1, int y1, int x2, int y2, int color);
        // void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color);
        // void DrawCircle(int x1, int y1, int x2, int y2, int color);

        void DrawText(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color);
        // void DrawImage(std::string image, int x, int y, int color);

        void DrawSprite(
            const std::string& filePath,
            const glm::vec2& position,
            const glm::vec2& size = glm::vec2(10.0f, 10.0f),
            float rotate = 0.0f,
            const glm::vec3& color = glm::vec3(1.0f)
        );
    }

    // todo add more graphics wrapper here
}
