//
// Created by insberr on 5/21/24.
//

#pragma once

#include <string>
#include <unordered_map>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <typeindex>
#include <memory>
#include "EngineTimer.h"
// Forward reference
class System;

class App {
public:
    App();
    ~App();

    int Run();

    GLFWwindow *GetWindow();

    template<typename T>
    void AddSystem(std::unique_ptr<T> system) {
        systems[typeid(T)] = std::move(system);
    }

    template <typename T>
    T* GetSystem() {
        auto it = systems.find(typeid(T));
        if (it != systems.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }

private:
    EngineTimer timer;

    int Init();
    int errorCode { 0 };
    GLFWwindow* window;

    std::unordered_map<std::type_index, std::unique_ptr<System>> systems;
};
