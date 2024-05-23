//
// Created by insberr on 5/22/24.
//

#pragma once
#include "System.h"

class InputSystem : public System {
public:
    InputSystem() : System("InputSystem") {};
    ~InputSystem() override;

    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Exit() override;


    bool IsKeyTriggered();
    bool IsKeyDown();
    bool IsKeyReleased();

private:
};
