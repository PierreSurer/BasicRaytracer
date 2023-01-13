#pragma once

#include "Object.hpp"

class Cylinder : public Object
{
public:
    Cylinder(glm::dvec3 position, glm::dvec3 rotation = {0.0, 0.0, 0.0}, double height = 0.5, double radius = 1.0);

    Hit intersect(const Ray &ray) const override;

    AABB getAABB() const override;

private:
    const glm::dvec3 position;
    const glm::dvec3 rotation;
    const double height;
    const double radius;

};
