#pragma once

#include "Object.hpp"

class Cylinder : public Object
{
public:
    Cylinder();

    std::unique_ptr<BaseHit> intersect(const Ray &ray) const override;

protected:
    AABB computeAABB() const override;

private:
    const glm::dvec3 position;
    const glm::dvec3 rotation;
    const double height;
    const double radius;

    glm::dmat3 orientation;
    glm::dmat3 inv_orientation;

};
