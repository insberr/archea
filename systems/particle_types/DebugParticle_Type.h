//
// Created by jonah on 6/1/2024.
//

#pragma once
#include "ParticleType.h"

const ParticleType DebugParticle = {
    .nameId = "Debug",
    .state = State::Gas,
    .color = { 255, 0, 0, 50 },
    .temperature = 0.0f,
    .movement = {}
};
