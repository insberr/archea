//
// Created by insberr on 5/21/24.
//

#pragma once

#include "../System.h"

class ParticleSystem : public System {
public:
    ParticleSystem() = default;
    ~ParticleSystem() = default;

    void Init() override;
    void Update(float dt) override;
    void Render(GLFWwindow* window) override;
    void Exit() override;
private:
    GLuint shaderProgram { 0 };
    GLuint VBO, VAO, EBO;
};

