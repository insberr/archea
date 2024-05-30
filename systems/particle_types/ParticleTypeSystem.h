//
// Created by insberr on 5/25/24.
//

#pragma once

#include "../System.h"
#include <vector>

typedef struct ParticleType ParticleType;

namespace ParticleTypeSystem {
    System AsSystem();

    /* Implementation Here */
    void AddParticleType(const ParticleType& particle);
    std::vector<std::array<unsigned char, 4>*> GetParticleColorIndexesForShader();
};
