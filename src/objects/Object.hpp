#pragma once

#include "Material.hpp"
#include "Raycast.hpp"

#include <memory>

using AABB = std::pair<glm::dvec3, glm::dvec3>;

class Object {
public:
    std::shared_ptr<Material> material = DEFAULT_MATERIAL;

    virtual std::unique_ptr<BaseHit> intersect(const Ray &ray) const = 0;

    virtual AABB computeAABB() const = 0;

protected:
    AABB aabb;
};
