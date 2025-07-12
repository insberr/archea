//
// Created by insberr on 5/27/24.
//

#pragma once
#include "System.h"

// Include OpenGL Stuff
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Graphics {
    System AsSystem();

    /* Implementation Here */

    // Some constants
    const unsigned WindowWidth = 1920;
    const unsigned WindowHeight = 1080;

    // Get the window value created by glfw
    GLFWwindow* GetWindow();

    // Generate a shader program
    GLuint CreateShaderProgram();

    // todo add more graphics wrapper here
}