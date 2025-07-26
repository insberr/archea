//
// Created by insberr on 7/12/25.
//

#pragma once
#include "SandboxScene.h"
#include "../systems/SceneSystem.h"

class MainMenuScene final : public SceneSystem::Scene {
public:
    MainMenuScene() : Scene("MainMenuScene") {
        SceneSystem::AddScene(new SandboxScene());
    };
    ~MainMenuScene() override;

    void Init() override;
    void InitGraphics() override;

    // void PrePollEvents() override;
    void Update(float dt) override;

    // void PreRender() override;
    void Render() override;
    // void PostRender() override;
    void Exit() override;
};
