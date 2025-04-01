//
// Created by jonah on 3/9/2025.
//

#pragma once
#include "ParticleType.h"

const ParticleType LavaParticle = {
    .nameId = "Lava",
    .state = State::Liquid,
    .color = {255, 127, 54, 240},
    .temperature = 30.0f,
    .getNextMove = [](ParticleMove::MoveState& lastMoveState) {
        switch (lastMoveState.step) {
        case 0: return ParticleMove::predefined::randomLR_Down(lastMoveState);
        case 1: return ParticleMove::predefined::down(lastMoveState);
        case 2: return ParticleMove::predefined::randomLRFB(lastMoveState);
        default: { lastMoveState.done = true; return; }
        }

    },
};

