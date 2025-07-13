//
// Created by insberr on 7/12/25.
//

#pragma once
#include "../systems/SceneSystem.h"

class MainMenuScene final : public SceneSystem::Scene {
public:
    MainMenuScene() : Scene("MainMenuScene") {};
    ~MainMenuScene() override;

    void Init() override;

    // void PrePollEvents() override;
    void Update(float dt) override;

    // void PreRender() override;
    void Render() override;
    // void PostRender() override;
    void Exit() override;
};
