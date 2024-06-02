//
// Created by insberr on 5/23/24.
//

#pragma once

enum ParticleMovement {
    Down,
    Up,
    Left,
    Right
};

enum State {
    Solid,
    Liquid,
    Gas,
};

struct ParticleType {
    const char* nameId;

    State state;
    unsigned char color[4];
    float temperature;
    ParticleMovement movement[];
};
