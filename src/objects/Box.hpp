#pragma once

#include "Object.hpp"

class Box : public Object
{
public:
    Box(glm::dvec3 position, glm::dvec3 rotation = {0.0, 0.0, 0.0}, double size = 1.0)
     : position(position), rotation(rotation), size(size) { }

    Hit intersect(const Ray &ray) const override;

    const glm::dvec3 position;
    const glm::dvec3 rotation;
    const double size;

};
