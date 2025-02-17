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

    static bool IsInBounds(const Bounds& bounds, const glm::uvec3& position) {
        return
            position.x >= bounds.min.x && position.x <= bounds.max.x &&
                position.y >= bounds.min.y && position.y <= bounds.max.y &&
                    position.z >= bounds.min.z && position.z <= bounds.max.z;
    }



    std::vector<unsigned> Chunk::asParticleTypes3DArray() const {
        std::vector<unsigned int> particles(dimensions.x * dimensions.y * dimensions.z);
        std::mdspan<unsigned int, size_max_extents> particlesView = std::mdspan(particles.data(), dimensions.z, dimensions.y, dimensions.x);

        // todo: This is very wrong. There must be a better way to have a view into the data?
        //  Maybe it isn't right for there to be a global thing of the data?
        for (const auto& [position, particle] : *particlesMap) {
            if (!IsInBounds(bounds, position)) {
                continue;
            }
            const auto relativePosition = position - bounds.min;
            particlesView[Vec3ToArray(relativePosition)] = particle.particleType;
        }

        return std::move(particles);
    }

    ParticlesInfoHashMap Chunk::asClonedMap() const {
        ParticlesInfoHashMap particles;

        for (const auto& [position, particle] : *particlesMap) {
            // todo: use relative position?
            // const auto relativePosition = position - bounds.min;
            particles.insert(std::make_pair(position, particlesMap->at(position)));
        }

        return std::move(particles);
    }

    void Chunk::loadMap(const ParticlesInfoHashMap& _particlesMap) const {
        for (const auto& [position, particleInfo] : _particlesMap) {
            if (!IsInBounds(bounds, position)) {
                throw std::invalid_argument("Particle map contained a value that is not within the bound of the chunk. (TODO: Print value)");
            }
            particlesMap->insert_or_assign(position, particleInfo);
        }
    }

    bool Chunk::containsParticle(const glm::ivec3& position) const {
        return particlesMap->contains(position);
    }
    ParticleInfo& Chunk::getParticle(const glm::ivec3& position) const {
        if (!IsInBounds(bounds, position)) {
            throw std::invalid_argument("Position (TODO) is out of this chunks bounds.");
        }
        // todo: return air or just let it throw the exception?
        return particlesMap->at(position);
    }
    void Chunk::moveParticle(const glm::ivec3& position, const glm::ivec3& newPosition) const {
        if (!IsInBounds(bounds, position) || !IsInBounds(bounds, newPosition)) {
            throw std::invalid_argument("Position or new position out of bounds of the chunk");
        }

        auto p = particlesMap->extract(position);
        p.key() = newPosition;
        particlesMap->insert_or_assign(p.key(), p.mapped());

    }
    void Chunk::addParticle(const glm::ivec3& position, const ParticleInfo& particleInfo) const {
        if (!IsInBounds(bounds, position)) {
            throw std::invalid_argument("Position out of chunk bounds");
        }
        particlesMap->insert_or_assign(position, particleInfo);
    }
    void Chunk::removeParticle(const glm::ivec3& position) const {
        if (!IsInBounds(bounds, position)) {
            throw std::invalid_argument("Position out of chunk bounds");
        }
        particlesMap->erase(position);
    }


    Chunk* Manager::getChunk(const glm::ivec3& position, const unsigned int& chunkSize) {
        auto* chunk = new Chunk(Bounds{position, position + glm::ivec3(chunkSize - 1)}, &particlesData);

        // todo: Maybe smart pointers would be better?
        dataChunks.emplace_back(chunk);

        return chunk;
    }

    ParticleInfo& Manager::Get(const glm::uvec3& position) {
        if (!IsPositionInBounds(position)) {
            throw std::exception("Position is out of bounds of ParticleDataManager dimensions.");
        }

        try {
            return particlesData.at(position);
        } catch (const std::out_of_range& e) {
            throw std::exception("No particle found at (todo position).");
            // todo: maybe just return air?
        }
    }

    bool Manager::Exists(const glm::uvec3& position) const {
        // Out of bounds, return false
        if (!IsPositionInBounds(position)) return false;

        return particlesData.contains(position);
    }

    void Manager::Swap(const glm::uvec3& position1, const glm::uvec3& position2) {
        if (!IsPositionInBounds(position1) || !IsPositionInBounds(position2)) {
            throw std::exception("Position 1 or 2 is out of bounds of ParticleDataManager dimensions.");
        }

        // todo: does this even work??
        std::swap(particlesData.at(position1), particlesData.at(position2));
    }

    void Manager::SetType(const glm::uvec3& position, unsigned particleType)
    {
        if (!IsPositionInBounds(position)) {
            throw std::exception("Position is out of bounds of ParticleDataManager dimensions.");
        }

        // todo: check if particleType is a valid particle type index
        Get(position).particleType = particleType;
    }

    // const std::vector<unsigned>& Manager::GetParticleTypesData() const
    // {
    //     // fixme: temp
    //     return std::vector<unsigned>(particlesData.begin(), particlesData.end());
    // }
    //
    // const std::vector<ParticleInfo>& Manager::GetParticleDataData() const
    // {
    //     // fixme: temp
    //     return std::vector<ParticleInfo>(particlesData.begin(), particlesData.end());
    // }

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
