//
// Created by jonah on 12/26/2024.
//

#include "OctreeNode.h"

#include "../systems/ParticleData.h"

void OctreeNode::subdivide() {
    glm::ivec3 mid = {
        (bounds.min + bounds.max) / 2,
    };

    children[0] = std::make_unique<OctreeNode>(BoundingBox{bounds.min, mid});
    children[1] = std::make_unique<OctreeNode>(
        BoundingBox{{mid.x, bounds.min.y, bounds.min.z}, {bounds.max.x, mid.y, mid.z}}
    );
    children[2] = std::make_unique<OctreeNode>(
        BoundingBox{{bounds.min.x, mid.y, bounds.min.z}, {mid.x, bounds.max.y, mid.z}}
    );
    children[3] = std::make_unique<OctreeNode>(
        BoundingBox{{mid.x, mid.y, bounds.min.z}, {bounds.max.x, bounds.max.y, mid.z}}
    );
    children[4] = std::make_unique<OctreeNode>(
        BoundingBox{{bounds.min.x, bounds.min.y, mid.z}, {mid.x, mid.y, bounds.max.z}}
    );
    children[5] = std::make_unique<OctreeNode>(
        BoundingBox{{mid.x, bounds.min.y, mid.z}, {bounds.max.x, mid.y, bounds.max.z}}
    );
    children[6] = std::make_unique<OctreeNode>(
        BoundingBox{{bounds.min.x, mid.y, mid.z}, {mid.x, bounds.max.y, bounds.max.z}}
    );
    children[7] = std::make_unique<OctreeNode>(BoundingBox{mid, bounds.max});

    isDivided = true;
}

bool OctreeNode::insert(const glm::ivec3& point, const ParticleData::InternalData& value) {
    if (!bounds.contains(point)) {
        return false;
    }

    if (data.size() < capacity) {
        data.emplace_back(point, value);
        return true;
    }

    if (!isDivided) {
        subdivide();
    }

    for (const auto& child : children) {
        if (child->insert(point, value)) {
            return true;
        }
    }

    return false;
}

bool OctreeNode::remove(const glm::ivec3& point) {
    auto it = std::find_if(
        data.begin(),
        data.end(),
        [&](const auto& pair) {
            return pair.first == point;
        }
    );

    if (it != data.end()) {
        data.erase(it);
        return true;
    }

    if (isDivided) {
        for (auto& child : children) {
            if (child->remove(point)) {
                return true;
            }
        }
    }

    return false;
}

std::optional<ParticleData::InternalData> OctreeNode::get(const glm::ivec3& point) const {
    for (const auto& pair : data) {
        if (pair.first == point) {
            return pair.second;
        }
    }

    if (isDivided) {
        for (const auto& child : children) {
            if (child) {
                auto result = child->get(point);
                if (result) {
                    return result;
                }
            }
        }
    }

    return std::nullopt;
}

bool OctreeNode::move(const glm::ivec3& oldPoint, const glm::ivec3& newPoint, const ParticleData::InternalData& value) {
    if (!bounds.contains(oldPoint)) return false;

    auto it = std::find_if(
        data.begin(),
        data.end(),
        [&](const auto& pair) {
            return pair.first == oldPoint;
        }
    );

    if (it != data.end()) {
        if (bounds.contains(newPoint)) {
            it->first = newPoint;
            return true;
        } else {
            data.erase(it);
            return insert(newPoint, value);
        }
    }

    if (isDivided) {
        for (auto& child : children) {
            if (child->bounds.contains(oldPoint)) {
                return child->move(oldPoint, newPoint, value);
            }
        }
    }

    return false;
}

void OctreeNode::queryRange(
    const BoundingBox& range,
    std::vector<std::pair<glm::ivec3, ParticleData::InternalData>>& found
) const {
    if (!bounds.intersects(range)) {
        return;
    }

    for (const auto& pair : data) {
        if (range.contains(pair.first)) {
            found.push_back(pair);
        }
    }

    if (isDivided) {
        for (const auto& child : children) {
            if (child) {
                child->queryRange(range, found);
            }
        }
    }
}

void OctreeNode::getAll(std::vector<std::pair<glm::ivec3, ParticleData::InternalData>>& allData) const {
    allData.insert(allData.end(), data.begin(), data.end());

    if (isDivided) {
        for (const auto& child : children) {
            if (child) {
                child->getAll(allData);
            }
        }
    }
}
