#pragma once

#include "Object.hpp"

class Sphere : public Object
{
public:
    Sphere(glm::dvec3 position, double r, glm::dvec3 rotation = glm::dvec3(0.0));

    std::unique_ptr<BaseHit> intersect(const Ray &ray) const override;
    
protected:
    AABB computeAABB() const override;

private:
    const glm::dvec3 position;
    const glm::dvec3 rotation;
    const double r;

};
