//
// Created by insberr on 5/21/24.
//

#include "App.h"
#include "systems/ParticleSystem.h"

int main() {
    auto app = App{};

    // This gives all systems access to the app
    System::SetApp(&app);

    // Particle Simulation
    app.AddSystem(std::make_unique<ParticleSystem>());

    return app.Run();
}
