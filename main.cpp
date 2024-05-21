//
// Created by insberr on 5/21/24.
//

#include <iostream>
#include "App.h"
#include "systems/triangl_render.h"

int main() {
    auto app = App{};

    // Test triangle system
    auto triangle_sys = triangl_render();
    app.AddSystem(&triangle_sys);

    return app.Run();
}
