#pragma once

#include "Material.hpp"
#include "Raycast.hpp"

#include <memory>

using AABB = std::pair<glm::dvec3, glm::dvec3>;

class Object {
public:
    std::shared_ptr<Material> material = DEFAULT_MATERIAL;
    glm::dvec3 angularVelocity = glm::dvec3(0.0); //in rad/s
    glm::dvec3 velocity = glm::dvec3(0.0); //in m/s

    virtual ~Object() = default;

    virtual std::unique_ptr<BaseHit> intersect(const Ray &ray) const = 0;

    virtual AABB computeAABB() const = 0;

    void setModel(glm::mat4 model);

protected:
    glm::dmat4 model = glm::dmat4(1.0);
    glm::dmat4 invModel = glm::dmat4(1.0);
    glm::dmat3 normModel = glm::dmat4(1.0); // transpose inverse (normal matrix)
    Ray computeLocalRay(const Ray& globalRay) const;
};
