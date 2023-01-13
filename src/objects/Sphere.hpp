#pragma once

#include "Object.hpp"

class Sphere : public Object
{
public:
    Sphere(glm::dvec3 position, double r);

    Hit intersect(const Ray &ray) const override;
    
    AABB getAABB() const override;

private:
    const glm::dvec3 position;
    const double r;
};
