#pragma once

#include "Object.hpp"

class Sphere : public Object
{
public:
    Sphere();

    std::unique_ptr<BaseHit> intersect(const Ray &ray) const override;
    
protected:
    AABB computeAABB() const override;
};
