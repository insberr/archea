//
// Created by insberr on 5/21/24.
//

#include <iostream>
#include "App.h"
#include "systems/ParticleSystem.h"

int main() {
    auto app = App{};

    // Test triangle system
    auto particleSystem = ParticleSystem{};
    app.AddSystem(&particleSystem);

    return app.Run();
}
