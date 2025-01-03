//
// Created by jonah on 12/26/2024.
//

#include "Octree.h"

#include <memory>
#include <vector>
#include <optional>

#include "OctreeNode.h"

#include "../systems/ParticleData.h"

Octree::Octree(const BoundingBox& bounds, size_t capacity) :
    root(std::make_unique<OctreeNode>(bounds, capacity)) {}

bool Octree::insert(const glm::ivec3& point, const ParticleData::InternalData& value) {
    return root->insert(point, value);
}

bool Octree::remove(const glm::ivec3& point) {
    return root->remove(point);
}

std::optional<ParticleData::InternalData> Octree::get(const glm::ivec3& point) const {
    return root->get(point);
}

bool Octree::move(const glm::ivec3& oldPoint, const glm::ivec3& newPoint, const ParticleData::InternalData& value) {
    return root->move(oldPoint, newPoint, value);
}

std::vector<std::pair<glm::ivec3, ParticleData::InternalData>> Octree::queryRange(const BoundingBox& range) const {
    std::vector<std::pair<glm::ivec3, ParticleData::InternalData>> found;
    root->queryRange(range, found);
    return found;
}

std::vector<std::pair<glm::ivec3, ParticleData::InternalData>> Octree::getAll() const {
    std::vector<std::pair<glm::ivec3, ParticleData::InternalData>> allData;
    root->getAll(allData);
    return allData;
}
