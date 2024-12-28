#pragma once

#include <memory>
#include <optional>
#include <vector>

#include <glm/vec3.hpp>

namespace ParticleData {
    struct InternalData;
}

struct BoundingBox;
class OctreeNode;

class Octree {
    std::unique_ptr<OctreeNode> root;

public:
    explicit Octree(const BoundingBox& bounds, size_t capacity = 4);

    bool insert(const glm::ivec3& point, const ParticleData::InternalData& value);

    bool remove(const glm::ivec3& point);

    std::optional<ParticleData::InternalData> get(const glm::ivec3& point) const;

    bool move(const glm::ivec3& oldPoint, const glm::ivec3& newPoint, const ParticleData::InternalData& value);

    std::vector<std::pair<glm::ivec3, ParticleData::InternalData>> queryRange(const BoundingBox& range) const;

    std::vector<std::pair<glm::ivec3, ParticleData::InternalData>> getAll() const;
};

// int main() {
//     BoundingBox bounds{{0, 0, 0}, {100, 100, 100}};
//     Octree<int> octree(bounds);
//
//     octree.insert({10, 10, 10}, 42);
//     octree.insert({50, 50, 50}, 99);
//
//     auto value = octree.get({10, 10, 10});
//     if (value) {
//         std::cout << "Value at (10, 10, 10): " << *value << std::endl;
//     }
//
//     octree.remove({10, 10, 10});
//     value = octree.get({10, 10, 10});
//     if (!value) {
//         std::cout << "Value at (10, 10, 10) removed successfully." << std::endl;
//     }
//
//     octree.insert({20, 20, 20}, 88);
//     octree.move({20, 20, 20}, {30, 30, 30}, 88);
//
//     auto range = octree.queryRange({{0, 0, 0}, {40, 40, 40}});
//     std::cout << "Points in range (0, 0, 0) to (40, 40, 40):\n";
//     for (const auto& [point, val] : range) {
//         std::cout << "  Point(" << point.x << ", " << point.y << ", " << point.z << ") = " << val << std::endl;
//     }
//
//     auto allData = octree.getAll();
//     std::cout << "All points in the octree:\n";
//     for (const auto& [point, val] : allData) {
//         std::cout << "  Point(" << point.x << ", " << point.y << ", " << point.z << ") = " << val << std::endl;
//     }
//
//     return 0;
// }
