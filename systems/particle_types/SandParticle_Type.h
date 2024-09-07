//
// Created by insberr on 5/25/24.
//

#pragma once
#include "ParticleType.h"

const ParticleType SandParticle = {
    .nameId = "Sand",
    .state = State::Solid,
    .color = {200, 150, 10, 255},
    .temperature = 30.0f,
    .getNextMove = [](ParticleMove::MoveState& lastMoveState) {
        switch (lastMoveState.step) {
            case 0: return ParticleMove::predefined::randomLR_Down(lastMoveState);
            case 1: return ParticleMove::predefined::down(lastMoveState);
            default: { lastMoveState.done = true; return; }
        }

    },
};
