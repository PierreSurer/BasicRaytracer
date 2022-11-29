#include "Box.hpp"
#include <math.h>

Hit Box::intersect(const Ray &ray) const
{
    glm::dvec3 tMin = ((position - size) - ray.O) / ray.D;
    glm::dvec3 tMax = ((position + size) - ray.O) / ray.D;
    glm::dvec3 t1;
    glm::dvec3 t2;
    t1 = min(tMin, tMax);
    t2 = max(tMin, tMax);
    double tNear = std::max(std::max(t1.x, t1.y), t1.z);
    double tFar = std::min(std::min(t2.x, t2.y), t2.z);
    if(tNear > tFar) return Hit::NO_HIT();
    
    glm::dvec3 intersectionPoint = ray.at(tNear);
    glm::dvec3 N = normalize(intersectionPoint - position);

    return Hit(tNear, N);
}
