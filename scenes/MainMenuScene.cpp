//
// Created by insberr on 7/12/25.
//

#include "MainMenuScene.h"
#include "../App.h"
#include "imgui.h"
#include "SandboxScene.h"

void MainMenuScene::Init() {
    SceneSystem::AddScene(new SandboxScene());
}

void MainMenuScene::Update(float dt) {
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
}

MainMenuScene::~MainMenuScene() {
}
