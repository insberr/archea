//
// Created by insberr on 7/12/25.
//

#pragma once
#include "../systems/SceneSystem.h"

// TODO: Move these and other static values somewhere else
namespace SandboxVars {
    static float particleScale { 0.5f };
    static unsigned int chunkSize { 64 };
}

class SandboxScene final : public SceneSystem::Scene {
public:
    SandboxScene() : Scene("SandboxScene") {};
    ~SandboxScene() override;

    void Init() override;
    void InitGraphics() override;

    // void PrePollEvents() override;
    void Update(float dt) override;

    // void PreRender() override;
    void Render() override;
    // void PostRender() override;
    void Exit() override;
};

