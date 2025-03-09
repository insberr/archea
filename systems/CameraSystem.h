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

namespace CameraSystem {
    System AsSystem();

    /* Implementation Here */

    /* Camera Functions */
    glm::vec3 GetPosition();
    glm::vec3 GetTarget();

    void set(const glm::vec3& position, const glm::vec3& target);
    void setAspectRatio(float newAspectRatio);

    glm::mat4 CameraMatrix();

    void Reset();

    float GetFOV();

    glm::mat4 GetModel();
    glm::mat4 GetView();
    glm::mat4 GetProjection();
};
