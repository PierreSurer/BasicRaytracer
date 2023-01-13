#pragma once

#include "Light.hpp" //TODO Ray and Hit in Light.hpp??????

#include <memory>

class Material;

using AABB = std::pair<glm::dvec3, glm::dvec3>;

class Object {
public:
    std::shared_ptr<Material> material;

    virtual Hit intersect(const Ray &ray) const = 0;

    virtual AABB computeAABB() const = 0;

protected:
    AABB aabb;
};
