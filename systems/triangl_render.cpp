//
// Created by insberr on 5/21/24.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "triangl_render.h"

// Loads contents of shader file into a string
std::string readShaderFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShaders() {
    // Read Shdaer files
    std::string vertexShaderSource = readShaderFile("shaders/vertex.glsl");
    std::string fragmentShaderSource = readShaderFile("shaders/fragment.glsl");

    // Build and compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSourceCStr = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexSourceCStr, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSourceCStr = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentSourceCStr, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

triangl_render::triangl_render() {

}

triangl_render::~triangl_render() {

}

void triangl_render::Init() {
    // Create the shader program
    shaderProgram = compileShaders();

    // Generate and bind the vertex buffers
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void triangl_render::Update(float dt) {

}

void triangl_render::Render() {
    // Set the shader program
    glUseProgram(shaderProgram);

    // Bind the vertex data
    glBindVertexArray(VAO);

    // Call draw
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void triangl_render::Exit() {

}
