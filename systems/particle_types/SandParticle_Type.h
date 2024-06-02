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
    .movement = {
        Particle::Down,
        Particle::Down + Particle::Right,
        Particle::Down + Particle::Left,
        Particle::Down + Particle::Forward,
        Particle::Down + Particle::Backward,
    }
};
