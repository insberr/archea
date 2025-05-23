//
// Created by jonah on 6/1/2024.
//

#pragma once
#include "ParticleType.h"
#include "ParticleMove.h"

const ParticleType DebugParticle = {
    .nameId = "Debug",
    .state = State::Gas,
    .color = { 131, 50, 168, 50 },
    .temperature = 0.0f,
    .getNextMove = ParticleMove::predefined::none,
};
