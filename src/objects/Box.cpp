#include "Box.hpp"
#include <math.h>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtx/extended_min_max.hpp>
#include <glm/gtx/component_wise.hpp>

using namespace glm;

Box::Box(glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 size)
    : position(position), rotation(rotation), size(size / 2.0)
{ 
    dquat rot = dquat(rotation);
    orientation = mat3_cast(rot);
    inv_orientation = inverse(orientation);
}

std::unique_ptr<BaseHit> Box::intersect(const Ray &ray) const
{
    dvec3 localOrigin = inv_orientation * (ray.O - position);
    dvec3 localDirection = inv_orientation * ray.D;

    dvec3 tMin = ((-size) - localOrigin) / localDirection;
    dvec3 tMax = ((+size) - localOrigin) / localDirection;
    dvec3 t1 = min(tMin, tMax);
    dvec3 t2 = max(tMin, tMax);
    double tNear = compMax(t1);
    double tFar = compMin(t2);
    if(tNear > tFar || tFar < 0.0) return Hit::NO_HIT();
    
    dvec3 N;

    if(tNear >= 0.0) { // if outside box
        if(t1.x == tNear)      N = {-sign(localDirection.x), 0.0, 0.0};
        else if(t1.y == tNear) N = {0.0, -sign(localDirection.y), 0.0};
        else                   N = {0.0, 0.0, -sign(localDirection.z)};
    } else {
        if(t2.x == tFar)       N = {sign(localDirection.x), 0.0, 0.0};
        else if(t2.y == tFar)  N = {0.0, sign(localDirection.y), 0.0};
        else                   N = {0.0, 0.0, sign(localDirection.z)};
    }

    return std::make_unique<Hit>(tNear, HitParams{ this, orientation * N, dvec3(0.0) }); // TODO uvs

    
}

AABB Box::computeAABB() const {
    AABB aabb;
    dvec3 s0 = orientation * dvec3(size.x / 2.0, size.x / 2.0, size.x / 2.0);
    dvec3 s1 = orientation * dvec3(size.x / 2.0, size.x / 2.0, -size.x / 2.0);
    dvec3 s2 = orientation * dvec3(size.x / 2.0, -size.x / 2.0, size.x / 2.0);
    dvec3 s3 = orientation * dvec3(size.x / 2.0, -size.x / 2.0, -size.x / 2.0);
    dvec3 s4 = orientation * dvec3(-size.x / 2.0, size.x / 2.0, size.x / 2.0);
    dvec3 s5 = orientation * dvec3(-size.x / 2.0, size.x / 2.0, -size.x / 2.0);
    dvec3 s6 = orientation * dvec3(-size.x / 2.0, -size.x / 2.0, size.x / 2.0);
    dvec3 s7 = orientation * dvec3(-size.x / 2.0, -size.x / 2.0, -size.x / 2.0);

    aabb.first = position + glm::min(glm::min(s0, s1, s2, s3), glm::min(s4, s5, s6, s7));
    aabb.second = position + glm::max(glm::max(s0, s1, s2, s3), glm::max(s4, s5, s6, s7));

    return aabb;
}