#pragma once

#include "Object.hpp"

class Cylinder : public Object
{
public:
    Cylinder();

    std::unique_ptr<BaseHit> intersect(const Ray &ray) const override;

protected:
    AABB computeAABB() const override;

private:
    const double height;
    const double radius;

};
