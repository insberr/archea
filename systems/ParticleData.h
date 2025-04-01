//
// Created by jonah on 9/7/2024.
//


#pragma once
#include <unordered_map>
#include <glm/vec3.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

struct ParticleType;

namespace ParticleData
{
    // struct IVec3Hash {
    //     std::size_t operator()(const glm::ivec3& v) const noexcept {
    //         // Combine the hash values of the three integers
    //         std::size_t h1 = std::hash<int>()(v.x);
    //         std::size_t h2 = std::hash<int>()(v.y);
    //         std::size_t h3 = std::hash<int>()(v.z);
    //         return h1 ^ (h2 << 1) ^ (h3 << 2); // Bit-shifting to reduce collisions
    //     }
    // };

    struct ParticleInformation {
        unsigned particleType;
        float temperature;
        glm::ivec3 velocity;
    };


    struct ParticleHashMap {
        using unordered_map = std::unordered_map<glm::ivec3, ParticleInformation>;

        unordered_map hashMap;

        ParticleInformation& get(const glm::ivec3& position);
        unordered_map& getAll();

        void add(const glm::ivec3& position, const ParticleInformation& particle);
        void add(const glm::ivec3& position, const unsigned& particleType);

        void remove(const glm::ivec3& position);

        bool exists(const glm::ivec3& position) const;

        void swap(const glm::ivec3& position1, const glm::ivec3& position2);
    };
}
