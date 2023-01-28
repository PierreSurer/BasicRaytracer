#pragma once

#include "Object.hpp"


class Box : public Object
{
public:
    Box(glm::dmat4 model = glm::dmat4(1.0));

    std::unique_ptr<BaseHit> intersect(const Ray &ray) const override;

protected:
    AABB computeAABB() const override;
};
