#pragma once

#include "Object.hpp"

class Triangle : public Object
{
public:
    Triangle(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3) : p1(p1), p2(p2), p3(p3) { }

    Hit intersect(const Ray &ray) const override;

    // p1, p2, p3 must be in counter-clockwise order when looking at the visible side.
    const glm::dvec3 p1, p2, p3;
};
