//
// Created by insberr on 5/23/24.
//

#pragma once
#include <glm/vec3.hpp>
#include "ParticleMove.h"

enum State {
    Solid,
    Liquid,
    Gas,
};

struct NormColor {
    float r;
    float g;
    float b;
    float a;
};

struct ParticleColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    NormColor normalized() const
    {
        return {
            static_cast<float>(r) / 255.0f,
            static_cast<float>(g) / 255.0f,
            static_cast<float>(b) / 255.0f,
            static_cast<float>(a) / 255.0f,
        };
    }
};

struct ParticleType {
    const char* nameId;

    State state;
    ParticleColor color;
    float temperature;
    void (*getNextMove)(ParticleMove::MoveState&);
};
