#pragma once

#include "Object.hpp"

class Box : public Object
{
public:
    Box()
     : position(position), rotation(rotation), size(size) { }

    Hit intersect(const Ray &ray) const override;

    const glm::dvec3 position;
    const glm::dvec3 rotation;
    const glm::dvec3 size;

};
