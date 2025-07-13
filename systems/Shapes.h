//
// Created by jonah on 9/7/2024.
//

#pragma once
#include <GLFW/glfw3.h>

namespace Shapes::Cube {
    constexpr float cubeNormals[] = {
        // Front face
        0.0, 0.0, 1.0,
        // Back face
        0.0, 0.0, -1.0,
        // Top face
        0.0, 1.0, 0.0,
        // Bottom face
        0.0, -1.0, 0.0,
        // Right face
        1.0, 0.0, 0.0,
        // Left face
        -1.0, 0.0, 0.0
    };

    constexpr float cubeVertices[] = {
        // Front face
        -0.5f, -0.5f,  0.5f, // 0
        0.5f, -0.5f,  0.5f, // 1
        0.5f,  0.5f,  0.5f, // 2
        -0.5f,  0.5f,  0.5f, // 3

        // Back face
        -0.5f, -0.5f, -0.5f, // 4
        0.5f, -0.5f, -0.5f, // 5
        0.5f,  0.5f, -0.5f, // 6
        -0.5f,  0.5f, -0.5f, // 7

        // Top face
        0.5f,  0.5f,  0.5f, // 8
        -0.5f,  0.5f,  0.5f, // 9
        -0.5f,  0.5f, -0.5f, // 10
        0.5f,  0.5f, -0.5f, // 11

        // Bottom face
        -0.5f, -0.5f,  0.5f, // 12
        0.5f, -0.5f,  0.5f, // 13
        0.5f, -0.5f, -0.5f, // 14
        -0.5f, -0.5f, -0.5f, // 15

        // Right face
        0.5f, -0.5f,  0.5f, // 16
        0.5f, -0.5f, -0.5f, // 17
        0.5f,  0.5f, -0.5f, // 18
        0.5f,  0.5f,  0.5f, // 19

        // Left face
        -0.5f, -0.5f,  0.5f, // 20
        -0.5f, -0.5f, -0.5f, // 21
        -0.5f,  0.5f, -0.5f, // 22
        -0.5f,  0.5f,  0.5f, // 23
    };

    // Define the indices for the cube
    const GLuint indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Top face
        8, 9, 10,
        10, 11, 8,

        // Bottom face
        12, 13, 14,
        14, 15, 12,

        // Right face
        16, 17, 18,
        18, 19, 16,

        // Left face
        20, 21, 22,
        22, 23, 20
    };
}

namespace Shapes::Rect {
    constexpr float rectVertices[8] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    constexpr unsigned int rectIndices[6] = {
        0, 1, 2,
        2, 3, 0
    };
}
