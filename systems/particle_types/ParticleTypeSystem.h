//
// Created by insberr on 5/25/24.
//

#pragma once

#include <string>

#include "../System.h"
#include <vector>

struct NormColor;
struct ParticleType;

namespace ParticleTypeSystem {
    System AsSystem();

    /* Implementation Here */
    void AddParticleType(const ParticleType& particle);
    std::vector<NormColor> GetParticleColorIndexesForShader();

    ParticleType& GetParticleTypeInfo(unsigned int typeIndex);

    unsigned int GetParticleTypeCount();
};
