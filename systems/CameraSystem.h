//
// Created by insberr on 5/22/24.
//

#pragma once
#include "System.h"

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraSystem : public System {
public:
    CameraSystem() : System("Camera") {}
    ~CameraSystem() override;

    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Exit() override;

    /* Camera Functions */
    glm::vec3 GetPosition();
    glm::vec3 GetTarget();

    glm::mat4 CameraMatrix();

    void Reset();

    float GetFOV() const;

private:
    void Translate(glm::vec3 translation);

    glm::vec3 position;
    glm::vec3 target;
    float fieldOfView { 0.7f };

    float yaw { -90.0f };
    float pitch { 0.0f };
    float sensitivity { 0.3f };
    float travelSpeed { 5.0f };

    bool cursorLocked { false };
};
