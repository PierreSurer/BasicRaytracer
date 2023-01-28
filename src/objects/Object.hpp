#pragma once

#include "Material.hpp"
#include "Raycast.hpp"

#include <memory>

using AABB = std::pair<glm::dvec3, glm::dvec3>;

class Object {
public:
    std::shared_ptr<Material> material = DEFAULT_MATERIAL;

    virtual ~Object() = default;

    virtual std::unique_ptr<BaseHit> intersect(const Ray &ray) const = 0;

    virtual AABB computeAABB() const = 0;

    void setAngularVelocity(glm::dvec3 const& value) {angularVelocity = value;}
    void setVelocity(glm::dvec3 const& value) {velocity = value;}

protected:
    glm::dvec3 angularVelocity; //in rad/s
    glm::dvec3 velocity; //in m/s
};
