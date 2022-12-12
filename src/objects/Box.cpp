#include "Box.hpp"
#include <math.h>
#include <glm/gtx/quaternion.hpp>

Hit Box::intersect(const Ray &ray) const
{
    glm::dquat rot = glm::dquat(rotation);
    glm::dmat3 orientation = glm::mat3_cast(rot);
    glm::dvec3 localOrigin = glm::inverse(orientation) * (ray.O - position);
    glm::dvec3 localDirection = glm::inverse(orientation) * ray.D;

    glm::dvec3 tMin = ((- size) - localOrigin) / localDirection;
    glm::dvec3 tMax = ((size) - localOrigin) / localDirection;
    glm::dvec3 t1;
    glm::dvec3 t2;
    t1 = min(tMin, tMax);
    t2 = max(tMin, tMax);
    double tNear = std::max(std::max(t1.x, t1.y), t1.z);
    double tFar = std::min(std::min(t2.x, t2.y), t2.z);
    if(tNear > tFar) return Hit::NO_HIT();
    
    glm::dvec3 N;
    if(t1.x == tNear) N = {-glm::sign(localDirection.x), 0.0, 0.0};
    if(t1.y == tNear) N = {0.0, -glm::sign(localDirection.y), 0.0};
    if(t1.z == tNear) N = {0.0, 0.0, -glm::sign(localDirection.z)};

    return Hit(tNear, orientation * N);
}
