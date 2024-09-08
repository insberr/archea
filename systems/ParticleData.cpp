//
// Created by jonah on 9/7/2024.
//

#include "ParticleData.h"

#include <iostream>
#include <mdspan>
#include <array>

namespace ParticleData
{
    // Convert a uvec3 to an array
    static std::array<unsigned, 3> Vec3ToArray(const glm::uvec3& position)
    {
        return std::array{ position.z, position.y, position.x };
    }

    DataWrapper Manager::Get(const glm::uvec3& position) {
        if (!IsPositionInBounds(position)) {
            throw std::exception("Position is out of bounds of ParticleDataManager dimensions.");
        }

        return {
            position,
            view_particlesTypes[Vec3ToArray(position)],
            // cpp-reference says the operator[] takes three (in my case) values, but really it takes a span or array
            view_particlesData[Vec3ToArray(position)]
        };
    }

    bool Manager::Exists(const glm::uvec3& position) const {
        // Out of bounds, return false
        if (!IsPositionInBounds(position)) return false;

        if (view_particlesTypes[Vec3ToArray(position)] == 0) return false;

        return true;
    }

    void Manager::Swap(const glm::uvec3& position1, const glm::uvec3& position2) {
        if (!IsPositionInBounds(position1) || !IsPositionInBounds(position2)) {
            throw std::exception("Position 1 or 2 is out of bounds of ParticleDataManager dimensions.");
        }

        // Get both Types
        const unsigned position1Type = view_particlesTypes[Vec3ToArray(position1)];
        const unsigned position2Type = view_particlesTypes[Vec3ToArray(position2)];

        // Get both Data
        const InternalData position1Data = view_particlesData[Vec3ToArray(position1)];
        const InternalData position2Data = view_particlesData[Vec3ToArray(position2)];

        // Swap
        view_particlesTypes[Vec3ToArray(position1)] = position2Type;
        view_particlesTypes[Vec3ToArray(position2)] = position1Type;

        view_particlesData[Vec3ToArray(position1)] = position2Data;
        view_particlesData[Vec3ToArray(position2)] = position1Data;
    }

    void Manager::SetType(const glm::uvec3& position, unsigned particleType)
    {
        if (!IsPositionInBounds(position)) {
            throw std::exception("Position is out of bounds of ParticleDataManager dimensions.");
        }

        // todo: check if particleType is a valid particle type index

        view_particlesTypes[Vec3ToArray(position)] = particleType;
    }

    const std::vector<unsigned>& Manager::GetParticleTypesData() const
    {
        return particlesTypes;
    }

    const std::vector<InternalData>& Manager::GetParticleDataData() const
    {
        return particlesData;
    }

    unsigned Manager::GetCubicSize() const
    {
        return cubicSize;
    }

    const glm::uvec3& Manager::GetDimensions() const
    {
        return dimensions;
    }

    bool Manager::IsPositionInBounds(const glm::uvec3& position) const {
        if (
            position.x > dimensions.x ||
            position.y > dimensions.y ||
            position.z > dimensions.z
        ) {
            return false;
        }

        return true;
    }
}
