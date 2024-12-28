//
// Created by jonah on 12/26/2024.
//

#pragma once

#include <glm/vec3.hpp>

struct BoundingBox {
    glm::ivec3 min;
    glm::ivec3 max;

    bool contains(const glm::ivec3& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }

    bool intersects(const BoundingBox& other) const {
        return !(other.min.x > max.x || other.max.x < min.x ||
                 other.min.y > max.y || other.max.y < min.y ||
                 other.min.z > max.z || other.max.z < min.z);
    }
};
