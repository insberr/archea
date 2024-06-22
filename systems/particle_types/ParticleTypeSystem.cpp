//
// Created by insberr on 5/25/24.
//

#include "ParticleTypeSystem.h"
#include <algorithm>

#include "DebugParticle_Type.h"
#include "SandParticle_Type.h"

namespace ParticleTypeSystem {
    /* System Function Declarations */
    int Setup();
    System AsSystem() {
        return {
            Setup,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
        };
    }

    /* Private Variables And Functions */
    ParticleType noType {
        .nameId = "NoType",
        .state = State::Solid,
        .color = { 255, 255, 255, 255 },
        .temperature = 0,
        .getNextMove = ParticleMove::predefined::none,
    };

    std::vector<ParticleType> particleTypes;

}

int ParticleTypeSystem::Setup() {
    AddParticleType(DebugParticle);
    AddParticleType(SandParticle);

    return 0;
}

void ParticleTypeSystem::AddParticleType(const ParticleType &particle) {
    particleTypes.push_back(particle);
}

std::vector<NormColor> ParticleTypeSystem::GetParticleColorIndexesForShader()
{
    std::vector<NormColor> colors;
    for (const auto& particleType : particleTypes)
    {
        colors.push_back(particleType.color.normalized());
    }

    return colors;
}

ParticleType& ParticleTypeSystem::GetParticleTypeInfo(unsigned int typeIndex) {
    // Make sure the vector is not empty
    if (particleTypes.empty()) return noType;
    // The -1 is fine since it is not empty
    typeIndex = std::clamp(typeIndex, 0u, static_cast<unsigned>(particleTypes.size()) - 1);
    return particleTypes[typeIndex];
}
