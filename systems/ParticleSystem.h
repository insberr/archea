//
// Created by insberr on 5/21/24.
//

#pragma once

#include "System.h"
// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class ParticleSystem : public System {
public:
    ParticleSystem() : System("Particle") {};
    ~ParticleSystem() = default;

    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Exit() override;
private:
    std::vector<int> particles;
    GLuint shaderProgram { 0 };

    GLuint particlesBuffer { 0 };
    GLuint particlesColrosBuffer { 0 };

    GLuint VBO {0}, VAO {0}, EBO {0};

    // Settings
    int maxRaySteps { 200 };
};

