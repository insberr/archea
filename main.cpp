//
// Created by insberr on 5/21/24.
//

#include "App.h"
#include "systems/ParticleSystem.h"
#include "systems/InputSystem.h"

int main() {
    auto app = App{};

    // This gives all systems access to the app
    System::SetApp(&app);

    // Add the input system
    app.AddSystem(std::make_unique<InputSystem>());

    // Particle Simulation
    app.AddSystem(std::make_unique<ParticleSystem>());

    return app.Run();
}
