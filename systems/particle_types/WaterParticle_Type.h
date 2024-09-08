//
// Created by jonah on 9/7/2024.
//

#pragma once
#include "ParticleType.h"

const ParticleType WaterParticle = {
    .nameId = "Water",
    .state = State::Liquid,
    .color = {28, 171, 255, 200},
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
