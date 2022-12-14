#include "Box.hpp"
#include <math.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/component_wise.hpp>

using namespace glm;

Hit Box::intersect(const Ray &ray) const
{
    dquat rot = dquat(rotation);
    dmat3 orientation = mat3_cast(rot);
    dvec3 localOrigin = inverse(orientation) * (ray.O - position);
    dvec3 localDirection = inverse(orientation) * ray.D;

    dvec3 tMin = ((- size) - localOrigin) / localDirection;
    dvec3 tMax = ((size) - localOrigin) / localDirection;
    dvec3 t1 = min(tMin, tMax);
    dvec3 t2 = max(tMin, tMax);
    double tNear = compMax(t1);
    double tFar = compMin(t2);
    if(tNear > tFar || tNear < 0.0) return Hit::NO_HIT();
    
    dvec3 N;
    if(t1.x == tNear)      N = {-sign(localDirection.x), 0.0, 0.0};
    else if(t1.y == tNear) N = {0.0, -sign(localDirection.y), 0.0};
    else                   N = {0.0, 0.0, -sign(localDirection.z)};

    return Hit(tNear, orientation * N);
}
