#pragma once

#include "Raycast.hpp"
#include "Object.hpp"
#include <glm/glm.hpp>

class Sphere : public Object
{
public:
    Sphere(glm::dvec3 position, double r);

    Hit intersect(const Ray &ray) const override;

    glm::dvec3 position;
    double r;
};
