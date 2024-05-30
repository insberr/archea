//
// Created by insberr on 5/25/24.
//

#pragma once
#include "ParticleType.h"

// Why I don't need to import the particle type header and this still recognizes the type is beyond me
const ParticleType SandParticle = {
    .state = ParticleType::State::Solid,
    .color = {200, 150, 10, 255},
    .temperature = 30.0f
};

