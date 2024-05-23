//
// Created by insberr on 5/23/24.
//

#pragma once
#include "System.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class ImGuiSystem : public System {
public:
    ImGuiSystem() : System("ImGuiSystem") {}
    ~ImGuiSystem() override;

    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Exit() override;

    void StartFrame();
    void EndFrame();
private:

};
