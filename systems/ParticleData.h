//
// Created by jonah on 9/7/2024.
//


#pragma once
#include <mdspan>
#include <vector>
#include <glm/vec3.hpp>

struct ParticleType;

namespace ParticleData
{
    struct InternalData {
        glm::vec3 realPosition;
        float temperature;
    };

    struct DataWrapper {
        glm::uvec3 position;
        unsigned particleType;
        const InternalData& data;
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

            particlesTypes.resize(cubicSize, 0);
            view_particlesTypes = std::mdspan(particlesTypes.data(), dimensions.x, dimensions.y, dimensions.z);

            particlesData.resize(cubicSize);
            view_particlesData = std::mdspan(particlesData.data(), dimensions.x, dimensions.y, dimensions.z);
        };

        DataWrapper Get(const glm::uvec3& position);

        /*
         * Check if a non-"None" particle exists at the position.
         * If out of bounds, returns false.
         */
        bool Exists(const glm::uvec3& position) const;

        // Swap two particles
        void Swap(const glm::uvec3& position1, const glm::uvec3& position2);

        void SetType(const glm::uvec3& position, unsigned particleType);

        /* Access the raw data if needed */
        const std::vector<unsigned>& GetParticleTypesData() const;
        const std::vector<InternalData>& GetParticleDataData() const;
    private:
        const unsigned cubicSize;
        const glm::uvec3 dimensions;

        using size_max_extents = std::extents<size_t, SIZE_MAX, SIZE_MAX, SIZE_MAX>;

        // This is for the gpu. The int represents the particle type index
        std::vector<unsigned> particlesTypes;
        std::mdspan<unsigned, size_max_extents> view_particlesTypes;

        // This is for the cpu. Extra data abou particles that isn't needed by the gpu
        std::vector<InternalData> particlesData;
        std::mdspan<InternalData, size_max_extents> view_particlesData;

        bool IsPositionInBounds(const glm::uvec3& position) const;
    };
}
