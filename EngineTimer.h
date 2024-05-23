//
// Created by jonah on 4/2/2024.
//

#pragma once

#include <chrono>

class EngineTimer {
public:
    EngineTimer();
    float Mark();
    float Peek() const;
private:
    std::chrono::steady_clock::time_point last;
};
