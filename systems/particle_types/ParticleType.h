//
// Created by insberr on 5/23/24.
//

#pragma once
#include <glm/vec3.hpp>


namespace Particle {
    using namespace glm;

    // y -1
    const vec3 Down = vec3(0, -1, 0);
    // y +1
    const vec3 Up = vec3(0, 1, 0);
    // x -1
    const vec3 Left = vec3(-1, 0, 0);
    // x +1
    const vec3 Right = vec3(1, 0, 0);
    // z -1
    const vec3 Forward = vec3(0, 0, -1);
    // z +1
    const vec3 Backward = vec3(0, 0, 1);

}

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
    std::vector<glm::vec3> movement;
};
