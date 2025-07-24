//
// Created by insberr on 7/12/25.
//

#pragma once
#include <string>

#include "System.h"

namespace SceneSystem {
    System AsSystem();

    /* Scene Class */
    // struct System {
    //     // Called when the scene is added to the scene system
    //     int (*Setup)() = nullptr;
    //     // Called when the scene becomes active
    //     void (*Init)() = nullptr;
    //
    //     // This is a pre-update function. Called before glPollEvents
    //     void (*PrePollEvents)() = nullptr;
    //     // Update is called every game loop
    //     void (*Update)(float dt) = nullptr;
    //
    //     void (*PreRender)() = nullptr;
    //     // Render is called every game loop
    //     void (*Render)() = nullptr;
    //     void (*PostRender)() = nullptr;
    //
    //     // Called when the scene becomes inactive
    //     void (*Exit)() = nullptr;
    //     // Called when the scene is destroyed
    //     void (*Done)() = nullptr;
    // };
    class Scene {
    public:
        explicit Scene(const std::string& sceneName) : name(sceneName) {};
        // Called when the scene is destroyed
        virtual ~Scene() = default;

        // Called when the scene becomes active
        virtual void Init() {};

        virtual void PrePollEvents() {};

        virtual void Update(float dt) {};

        virtual void PreRender() {};
        virtual void Render() {};
        virtual void PostRender() {};

        // Called when the scene becomes inactive
        virtual void Exit() {};

        const std::string name;
    };

    /* Implementation Here */
    void AddScene(Scene* scene);
    void RemoveScene(const std::string& sceneName);

    void SwitchActiveScene(const std::string& sceneName);
};
