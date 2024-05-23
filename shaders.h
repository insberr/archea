//
// Created by insberr on 5/21/24.
//

#pragma once

#include <string>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

std::string readShaderFile(const std::string& filePath);
GLuint compileShader(const std::string& shaderSource, GLenum shaderType);
GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
