//
// Created by insberr on 5/21/24.
//

#pragma once

#include "System.h"
#include <vector>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class App {
public:
    App();
    ~App();

    void AddSystem(System* system);

    int Run();
private:
    int Init();
    int errorCode { 0 };
    GLFWwindow* window;

    std::vector<System*> systems;
};
