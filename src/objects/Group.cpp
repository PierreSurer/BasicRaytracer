#include "Group.hpp"


Group::Group(ObjCollection objs)
    : objs(std::move(objs))
{ }

std::unique_ptr<BaseHit> Group::intersect(const Ray& ray) const
{
    std::unique_ptr<BaseHit> hit = Hit::NO_HIT();

    for (auto const& obj : objs) {
        auto thisHit = obj->intersect(ray);
        if (thisHit->t < hit->t) {
            hit = std::move(thisHit);
        }
    }

    return hit;
}

AABB Group::computeAABB() const
{
  AABB aabb;
  // TODO

  return aabb;
}