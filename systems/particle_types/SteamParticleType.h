//
// Created by jonah on 3/9/2025.
//

#pragma once
#include "ParticleType.h"

const ParticleType SteamParticle = {
    .nameId = "Steam",
    .state = State::Gas,
    .color = { 207, 207, 207, 200},
    .temperature = 30.0f,
    .getNextMove = [](ParticleMove::MoveState& lastMoveState) {
        switch (lastMoveState.step) {
        case 0: return ParticleMove::predefined::up(lastMoveState);
        case 1: return ParticleMove::predefined::randomLRFB(lastMoveState);
        default: { lastMoveState.done = true; return; }
        }

    },
};

