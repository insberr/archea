//
// Created by insberr on 5/25/24.
//

#include "ParticleTypeSystem.h"

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
    std::map<int, ParticleType> particleTypes;
}

int ParticleTypeSystem::Setup() { return 0; }
void ParticleTypeSystem::Init() {
    AddParticleType(SandParticle);
}
void ParticleTypeSystem::Update(float dt) {}
void ParticleTypeSystem::Render() {}
void ParticleTypeSystem::Exit() {}
void ParticleTypeSystem::Done() {}

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
