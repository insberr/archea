//
// Created by insberr on 5/25/24.
//

#pragma once
#include "../System.h"
#include "ParticleType.h"
#include <map>
#include <vector>

class ParticleTypeSystem : public System{
public:
    ParticleTypeSystem() : System("ParticleType") {}
    ~ParticleTypeSystem() override = default;

    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Exit() override;

    static void AddParticleType(const ParticleType& particle);
    std::vector<std::array<unsigned char, 4>*> GetParticleColorIndexesForShader();
private:
    static std::map<int, ParticleType> particleTypes;
};
