//
// Created by insberr on 5/21/24.
//

#include "App.h"

/* Systems */
#include "systems/GraphicsSystem.h"
#include "systems/InputSystem.h"
#include "systems/ImGuiSystem.h"
#include "systems/CameraSystem.h"
#include "systems/particle_types/ParticleTypeSystem.h"
#include "systems/ParticleSystem.h"


int main() {
    auto app = App{};

    // Graphics system, handles the window and api calls to gl
    int code = app.AddSystem(Graphics::AsSystem());
    if (code) return code;

    // Add the input system
    app.AddSystem(InputSystem::AsSystem());

    // ImGui System
    app.AddSystem(ImGuiSystem::AsSystem());

    // Camera System
    app.AddSystem(CameraSystem::AsSystem());

    // Add Particle Type System
    app.AddSystem(ParticleTypeSystem::AsSystem());
    // Particle Simulation
    app.AddSystem(ParticleSystem::AsSystem());

    return app.Run();
}
