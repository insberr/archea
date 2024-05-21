//
// Created by insberr on 5/21/24.
//

#pragma once

class System {
public:
    System() = default;
    ~System() = default;

    virtual void Init() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void Exit() = 0;
};
