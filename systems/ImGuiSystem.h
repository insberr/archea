//
// Created by insberr on 5/23/24.
//

#pragma once
#include "System.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace ImGuiSystem {
    System AsSystem();

    /* Implementation Here */

    void EnableImGui();
    void DisableImGui();
    bool IsImGuiEnabled();
};
