#pragma once

#include "Object.hpp"


class Box : public Object
{
public:
    Box();

    std::unique_ptr<BaseHit> intersect(const Ray &ray) const override;

protected:
    AABB computeAABB() const override;
};
