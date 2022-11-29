#pragma once

#include "Object.hpp"

class Sphere : public Object
{
public:
    Sphere(glm::dvec3 position, double r) : position(position), r(r) { }

    Hit intersect(const Ray &ray) const override;

    const glm::dvec3 position;
    const double r;
};
