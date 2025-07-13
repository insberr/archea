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
#include "systems/SceneSystem.h"
#include "systems/gui/GUI.h"


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

    app.AddSystem(GUI::AsSystem());

    // Add Particle Type System
    // TODO: Move into chunk manager or somewhere else that is not an engine system
    app.AddSystem(ParticleTypeSystem::AsSystem());

    app.AddSystem(SceneSystem::AsSystem());

    return app.Run();
}
