//
// Created by jonah on 9/7/2024.
//

#include "ParticleData.h"

namespace ParticleData
{
    ParticleInformation& ParticleHashMap::get(const glm::ivec3& position) {
        return hashMap[position];
    }

    ParticleHashMap::unordered_map& ParticleHashMap::getAll() {
        return hashMap;
    }

    void ParticleHashMap::add(const glm::ivec3& position, const ParticleInformation& particle) {
        hashMap[position] = particle;
    }
    void ParticleHashMap::add(const glm::ivec3& position, const unsigned& particleType) {
        hashMap[position] = {
            .particleType = particleType,
            .temperature = 32.0f
        };
    }

    void ParticleHashMap::remove(const glm::ivec3& position) {
        hashMap.erase(position);
    }

    bool ParticleHashMap::exists(const glm::ivec3& position) const {
        return hashMap.contains(position);
    }

    void ParticleHashMap::swap(const glm::ivec3& position1, const glm::ivec3& position2) {
        const auto position1_copy = get(position1);
        const auto position2_copy = get(position2);

        get(position1) = position2_copy;
        get(position2) = position1_copy;
    }
}
