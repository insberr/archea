//
// Created by jonah on 3/9/2025.
//

#pragma once
#include "ParticleType.h"

const ParticleType StoneParticle = {
    .nameId = "Stone",
    .state = State::Solid,
    .color = { 92, 92, 92, 255 },
    .temperature = 30.0f,
    .getNextMove = [](ParticleMove::MoveState& lastMoveState) {
        switch (lastMoveState.step) {
        case 0: return ParticleMove::predefined::randomLR_Down(lastMoveState);
        case 1: return ParticleMove::predefined::down(lastMoveState);
        default: { lastMoveState.done = true; return; }
        }
    },
};
