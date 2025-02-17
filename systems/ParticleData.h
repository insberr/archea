//
// Created by jonah on 9/7/2024.
//


#pragma once
#include <mdspan>
#include <unordered_map>
#include <vector>
#include <glm/vec3.hpp>

struct ParticleType;

namespace ParticleData
{
    // fixme: this probably doesnt belong here
    struct IVec3Hash {
        std::size_t operator()(const glm::ivec3& v) const noexcept {
            // Combine the hash values of the three integers
            std::size_t h1 = std::hash<int>()(v.x);
            std::size_t h2 = std::hash<int>()(v.y);
            std::size_t h3 = std::hash<int>()(v.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2); // Bit-shifting to reduce collisions
        }
    };

    struct ParticleInfo {
        glm::uvec3 position;
        unsigned particleType;
        float temperature;
    };

    struct Bounds {
        glm::ivec3 min;
        glm::ivec3 max;
    };

    using ParticlesInfoHashMap = std::unordered_map<glm::ivec3, ParticleInfo, IVec3Hash>;

    class Chunk {
        using size_max_extents = std::extents<size_t, SIZE_MAX, SIZE_MAX, SIZE_MAX>;
    public:
        Chunk(const glm::uvec3& dimensions, ParticlesInfoHashMap* data)
            : dimensions(dimensions), particlesMap(data)
        {
        }

        std::vector<unsigned int> asParticleTypes3DArray() const;
        ParticlesInfoHashMap asClonedMap() const;
        void loadMap(const ParticlesInfoHashMap& _particlesMap) const;

        const glm::uvec3& getDimensions() const { return dimensions; }
        int getCubicSize() const { return dimensions.x * dimensions.y * dimensions.z; }

        // Contains a non-air particle
        bool containsParticle(const glm::ivec3& position) const;
        ParticleInfo& getParticle(const glm::ivec3& position) const;
        void moveParticle(const glm::ivec3& position, const glm::ivec3& newPosition) const;
        void addParticle(const glm::ivec3& position, const ParticleInfo& particleInfo) const;
        void removeParticle(const glm::ivec3& position) const;

    private:
        const glm::uvec3 dimensions;
        ParticlesInfoHashMap* particlesMap;
    };

    class Manager {
    public:
        explicit Manager(const glm::uvec3& dimensions) :
            cubicSize(dimensions.x * dimensions.y * dimensions.z),
            dimensions(dimensions)
        {
            if (dimensions.x == 0 || dimensions.y == 0 || dimensions.z == 0) {
                throw std::exception("Cannot create ParticleDataManager with any dimension value being 0.");
            }
        };

        Chunk* getChunk(const glm::ivec3& position, const unsigned int& chunkSize);

        ParticleInfo& Get(const glm::uvec3& position);

        /*
         * Check if a non-"None" particle exists at the position.
         * If out of bounds, returns false.
         */
        bool Exists(const glm::uvec3& position) const;

        // Swap two particles
        void Swap(const glm::uvec3& position1, const glm::uvec3& position2);

        void SetType(const glm::uvec3& position, unsigned particleType);

        /* Access the raw data if needed */
        // const std::vector<unsigned>& GetParticleTypesData() const;
        // const std::vector<ParticleInfo>& GetParticleDataData() const;

        unsigned GetCubicSize() const;
        const glm::uvec3& GetDimensions() const;
    private:
        const unsigned cubicSize;
        const glm::uvec3 dimensions;

        // todo: should ParticleInfo be a pointer?
        std::unordered_map<glm::ivec3, ParticleInfo, IVec3Hash> particlesData;
        std::vector<Chunk*> dataChunks;

        bool IsPositionInBounds(const glm::uvec3& position) const;
    };
}
