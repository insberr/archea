//
// Created by insberr on 7/12/25.
//

#include "MainMenuScene.h"

#include <iostream>
#include <GL/glew.h>

#include "../App.h"
#include "imgui.h"
#include "SandboxScene.h"
#include "../systems/GraphicsSystem.h"
#include "../systems/gui/GUI.h"

static int sandboxButton = 0;
static int quitButton = 0;

void MainMenuScene::Init() {
    // 2d things dont like depth buffer yay ...
    glDisable(GL_DEPTH_TEST);

    sandboxButton = GUI::CreateButton(glm::vec2(1920/2, 1080/2), glm::vec2(300.0f, 100.0f), "Sandbox");
    quitButton = GUI::CreateButton(glm::vec2(1920/2, (1080/2) - 150), glm::vec2(300.0f, 100.0f), "Quit");
}

void MainMenuScene::Update(float dt) {
    if (Graphics::DidWindowResize()) {
        glm::ivec2 windowSize = Graphics::GetWindowSize();
        GUI::UpdateElementPosition(sandboxButton, glm::vec2(windowSize.x/2, windowSize.y/2));
        GUI::UpdateElementPosition(quitButton, glm::vec2(windowSize.x/2, windowSize.y/2 - 150));
    }

    const GUI::Element sandboxButtonElm = GUI::GetElementById(sandboxButton);
    if (sandboxButtonElm.isTriggered) {
        SceneSystem::SwitchActiveScene("SandboxScene");
    }

    const GUI::Element quitButtonElm = GUI::GetElementById(quitButton);
    if (quitButtonElm.isTriggered) {
        std::cout << "Quitting..." << std::endl;
        App::Shutdown();
    }
}

void MainMenuScene::Render() {}

void MainMenuScene::Exit() {
    GUI::RemoveElement(sandboxButton);
    GUI::RemoveElement(quitButton);

    // Re-enable depth test for because it is default
    glEnable(GL_DEPTH_TEST);
}

MainMenuScene::~MainMenuScene() = default;
