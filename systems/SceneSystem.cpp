//
// Created by insberr on 7/12/25.
//

#include "SceneSystem.h"

#include <ranges>
#include <unordered_map>

namespace SceneSystem {
    void PrePollEvents();
    void Update(float dt);
    void PreRender();
    void Render();
    void PostRender();
    void Exit();

    System AsSystem() {
        return {
            .PrePollEvents = PrePollEvents,
            .Update = Update,
            .PreRender = PreRender,
            .Render = Render,
            .PostRender = PostRender,
            .Exit = Exit,
        };
    }

    /* Private Variables And Functions */
    std::unordered_map<std::string, Scene*> Scenes;
    Scene* CurrentScene = nullptr;
    Scene* NextScene = nullptr;
};

void SceneSystem::AddScene(Scene* scene) {
    // TODO: Check and make sure nothing is already added for the scene
    Scenes.insert({ scene->name, scene });
}
void SceneSystem::RemoveScene(const std::string& sceneName) {
    const Scene* scene = Scenes.at(sceneName);
    delete scene;
    Scenes.erase(sceneName);
}

void SceneSystem::SwitchActiveScene(const std::string& sceneName) {
    try {
        NextScene = Scenes.at(sceneName);
        if (CurrentScene == nullptr) {
            CurrentScene = NextScene;
            NextScene = nullptr;

            CurrentScene->Init();
        }
    } catch (const std::out_of_range& e) {
        std::printf("Scene %s not found\n", sceneName.c_str());
        throw std::out_of_range("SceneSystem::SwitchActiveScene: Invalid Scene Name");
    }
}

/* System Implementation */

void SceneSystem::PrePollEvents() {
    CurrentScene->PrePollEvents();
}

void SceneSystem::Update(float dt) {
    if (NextScene != nullptr) {
        CurrentScene->Exit();
        CurrentScene = NextScene;
        NextScene = nullptr;

        CurrentScene->Init();
    }

    CurrentScene->Update(dt);
}

void SceneSystem::PreRender() {
    CurrentScene->PreRender();
}
void SceneSystem::Render() {
    CurrentScene->Render();
}
void SceneSystem::PostRender() {
    CurrentScene->PostRender();
}
void SceneSystem::Exit() {
    CurrentScene->Exit();

    for (auto &scene: Scenes | std::views::values) {
        delete scene;
        scene = nullptr;
    }
}
