#pragma once

#include "Object.hpp"


class Box : public Object
{
public:
    Box(glm::dvec3 position = glm::dvec3(.0), glm::dvec3 rotation = glm::dvec3(.0), glm::dvec3 size = glm::dvec3(.0));

    Hit intersect(const Ray &ray) const override;

    AABB getAABB() const override;

private:
    const glm::dvec3 position;
    const glm::dvec3 rotation;
    const glm::dvec3 size;

    glm::dmat3 orientation;
    glm::dmat3 inv_orientation;

};
