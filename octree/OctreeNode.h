//
// Created by jonah on 12/26/2024.
//

#pragma once

#include <memory>
#include <optional>
#include <vector>

#include <glm/vec3.hpp>

#include "BoundingBox.h"

namespace ParticleData {
    struct InternalData;
}

class OctreeNode {
public:
    BoundingBox bounds;
    std::vector<std::pair<glm::ivec3, ParticleData::InternalData>> data;
    std::unique_ptr<OctreeNode> children[8];
    const size_t capacity;
    bool isDivided;

    explicit OctreeNode(const BoundingBox& bounds, const size_t capacity = 4)
        : bounds(bounds), capacity(capacity), isDivided(false) {}

    void subdivide();

    bool insert(const glm::ivec3& point, const ParticleData::InternalData& value);

    bool remove(const glm::ivec3& point);

    std::optional<ParticleData::InternalData> get(const glm::ivec3& point) const;

    bool move(const glm::ivec3& oldPoint, const glm::ivec3& newPoint, const ParticleData::InternalData& value);

    void queryRange(const BoundingBox& range, std::vector<std::pair<glm::ivec3, ParticleData::InternalData>>& found) const;

    void getAll(std::vector<std::pair<glm::ivec3, ParticleData::InternalData>>& allData) const;
};
