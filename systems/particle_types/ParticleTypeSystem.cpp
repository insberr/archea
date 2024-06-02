//
// Created by insberr on 5/25/24.
//

#include "ParticleTypeSystem.h"

// Particle Types
#include <algorithm>
#include <array>

#include "DebugParticle_Type.h"
#include "SandParticle_Type.h"

namespace ParticleTypeSystem {
    /* System Function Declarations */
    int Setup();
    void Init();
    void Update(float dt);
    void Render();
    void Exit();
    void Done();
    System AsSystem() {
        return {
            Setup,
            Init,
            Update,
            Render,
            Exit,
            Done
        };
    }

    /* Private Variables And Functions */
    ParticleType noType {
        .nameId = "NoType",
        .state = State::Solid,
        .color = { 255, 255, 255, 255 },
        .temperature = 0,
        .movement = {},
    };

    std::vector<ParticleType> particleTypes;

}

int ParticleTypeSystem::Setup() { return 0; }
void ParticleTypeSystem::Init() {
    AddParticleType(DebugParticle);
    AddParticleType(SandParticle);
}
void ParticleTypeSystem::Update(float dt) {}
void ParticleTypeSystem::Render() {}
void ParticleTypeSystem::Exit() {}
void ParticleTypeSystem::Done() {}

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


