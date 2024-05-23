//
// Created by insberr on 5/21/24.
//

#pragma once

#include <utility>
#include <string>
class App;

class System {
protected:
    static App* app;
public:
    explicit System(std::string  systemName) : name(std::move(systemName)) {};
    virtual ~System() = default;

    virtual void Init() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void Exit() = 0;

    std::string name;

    // Set the app variable so all systems can access it
    static void SetApp(App* a);
};
