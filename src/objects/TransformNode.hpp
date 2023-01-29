#pragma once

#include "objects/Object.hpp"

class TransformNode : public Object {
public:
    TransformNode(std::unique_ptr<Object> obj, glm::dmat4 model);

    std::unique_ptr<BaseHit> intersect(const Ray &ray) const override;
    AABB computeAABB() const override;

    void setModel(glm::dmat4 model);
    void setLinearVelocity(glm::dvec3 vel);
    void setAngularVelocity(glm::dvec3 vel);

private:
    std::unique_ptr<Object> obj;

    // fixed transforms (position + rotation + scale)
    glm::dvec3 position = glm::dvec3(0.0);
    glm::dmat3 model = glm::dmat3(1.0);
    glm::dmat3 modelInv = glm::dmat3(1.0);

    // time-dependent transforms (velocities)
    glm::dvec3 velocity = glm::dvec3(0.0);
    glm::dvec3 angularVelocity = glm::dvec3(0.0);

    Ray computeLocalRay(const Ray& globalRay) const;
    glm::dmat3 computeModel(double time) const;
    glm::dmat3 computeModelInv(double time) const;

    friend class NodeHit;
};
