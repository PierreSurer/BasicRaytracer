#pragma once

#include "objects/Object.hpp"

class TransformNode : public Object {
public:
    TransformNode(std::unique_ptr<Object> obj, glm::dmat4 model);

    std::unique_ptr<BaseHit> intersect(const Ray &ray) const override;
    AABB computeAABB() const override;

    glm::dvec3 angularVelocity = glm::dvec3(0.0); //in rad/s
    glm::dvec3 velocity = glm::dvec3(0.0); //in m/s

    void setModel(glm::dmat4 model);

private:
    std::unique_ptr<Object> obj;

    glm::dmat4 model = glm::dmat4(1.0);
    glm::dmat4 invModel = glm::dmat4(1.0);
    glm::dmat3 normModel = glm::dmat4(1.0); // transpose inverse (normal matrix)

    Ray computeLocalRay(const Ray& globalRay) const;

    friend class NodeHit;
};
