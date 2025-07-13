//
// Created by insberr on 7/12/25.
//

#include "MainMenuScene.h"
#include "../App.h"
#include "imgui.h"
#include "SandboxScene.h"
#include "../systems/gui/GUI.h"

static int button = 0;

void MainMenuScene::Init() {
    button = GUI::CreateButton(glm::vec2(1920/2, 1080/2), glm::vec2(300.0f, 100.0f), "Sandbox");
}

void MainMenuScene::Update(float dt) {
    const GUI::Element buttonElm = GUI::GetElementById(button);
    // std::printf("Button ID: %d\nButton isHovered: %d\n", buttonElm.id, buttonElm.isHovered);
    if (buttonElm.isTriggered) {
        SceneSystem::SwitchActiveScene("SandboxScene");
    }
}

void MainMenuScene::Render() {
    if (ImGui::Begin("Main Menu")) {
        if (ImGui::Button("Sandbox")) {
            SceneSystem::SwitchActiveScene("SandboxScene");
        }
        if (ImGui::Button("Quit")) {
            App::Shutdown();
        }
    }
    ImGui::End();
}

void MainMenuScene::Exit() {
    GUI::RemoveElement(button);
}

MainMenuScene::~MainMenuScene() = default;
