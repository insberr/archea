//
// Created by insberr on 5/21/24.
//

#pragma once
#include "EngineTimer.h"

// Forward reference
class System;

class App {
public:
    App() = default;
    ~App();

    int Run();

    // Add a system to the engine.
    // Returns a status code from the system Setup function
    int AddSystem(const System& system);
private:
    EngineTimer timer;

    std::vector<System> systems;
};
