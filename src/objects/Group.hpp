#pragma once

#include "objects/Object.hpp"

#include <vector>
#include <memory>

// COMBAK: should we implement bvh here and make mesh inherit group ?
class Group : public Object
{
public:
    using ObjCollection = std::vector<std::unique_ptr<Object>>;

    Group(ObjCollection objs);

    virtual std::unique_ptr<BaseHit> intersect(const Ray &ray) const;

    virtual AABB computeAABB() const;

protected:
    ObjCollection objs;
};