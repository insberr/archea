//
// Created by insberr on 5/21/24.
//

#include "App.h"

/* Systems */
#include "systems/InputSystem.h"
#include "systems/CameraSystem.h"
#include "systems/ParticleSystem.h"

int main() {
    auto app = App{};

    // This gives all systems access to the app
    System::SetApp(&app);

    // Add the input system
    app.AddSystem(std::make_unique<InputSystem>());

    // Camera System
    app.AddSystem(std::make_unique<CameraSystem>());

    // Particle Simulation
    app.AddSystem(std::make_unique<ParticleSystem>());

    return app.Run();
}
