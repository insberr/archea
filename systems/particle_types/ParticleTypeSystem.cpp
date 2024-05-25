//
// Created by insberr on 5/25/24.
//

#include "ParticleTypeSystem.h"

#include "SandParticle_Type.h"

// Declare static variable
std::map<int, ParticleType> ParticleTypeSystem::particleTypes;

void ParticleTypeSystem::Init() {
    AddParticleType(SandParticle);
}
void ParticleTypeSystem::Update(float dt) {}
void ParticleTypeSystem::Render() {}
void ParticleTypeSystem::Exit() {}

void ParticleTypeSystem::AddParticleType(const ParticleType &particle) {
    // todo might be better to make the index based on the type enum
    auto size = particleTypes.size();
    particleTypes[size] = particle;
}

std::vector<std::array<unsigned char, 4>*> ParticleTypeSystem::GetParticleColorIndexesForShader() {
    std::vector<std::array<unsigned char, 4>*> colors;
    for (auto& [index, particle] : particleTypes) {
        // to do
    }
    return colors;
}
