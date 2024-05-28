//
// Created by insberr on 5/27/24.
//

#pragma once
#include "System.h"

// Include OpenGL Stuff
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Graphics {
    class InstanceClass : public System {
    public:
        InstanceClass() : System("Graphics") {}
        ~InstanceClass() override = default;

        int Setup() override;
        void Init() override;
        // don't need Update or Render
        void Update(float dt) override {};
        void Render() override {};
        void Exit() override;
        void Done() override;
    };
    InstanceClass& Instance();
    System* AsSystem();

    /* Implementation */

    // Some constants
    const unsigned WindowWidth = 1280;
    const unsigned WindowHeight = 720;

    int errorCode = 0;

    // Get the window value created by glfw
    GLFWwindow* GetWindow();

    // Generate a shader program
    GLuint CreateShaderProgram();

    // todo add more graphics wrapper here
}