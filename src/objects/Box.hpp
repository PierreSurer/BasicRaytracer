#pragma once

#include "Object.hpp"

class Box : public Object
{
public:
    Box(glm::dvec3 position, double size) : position(position), size(size) { }

    Hit intersect(const Ray &ray) const override;

    const glm::dvec3 position;
    const double size;

};
