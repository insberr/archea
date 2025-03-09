//
// Created by jonah on 3/8/2025.
//

#pragma once
#include "ParticleType.h"
#include "ParticleMove.h"

const ParticleType ErrorParticle = {
    .nameId = "Error",
    .state = State::Solid,
    .color = { 255, 0, 0, 50 },
    .temperature = 0.0f,
    .getNextMove = ParticleMove::predefined::none,
};
