#include "Sphere.hpp"

#include <glm/glm.hpp>

using namespace glm;

Sphere::Sphere(dvec3 position, double r)
    : position(position), r(r)
{ }

Hit Sphere::intersect(const Ray& ray) const
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    * 
    * Insert calculation of ray/sphere intersection here. 
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return Hit::NO_HIT().
    * Otherwise, return an instance of Hit() with the distance of the
    * intersection point from the ray origin as t and the normal ad N (see example).
    ****************************************************/

    // place holder for actual intersection calculation

    dvec3 OC = normalize(position - ray.O);
    if (dot(OC, ray.D) < 0.999) {
        return Hit::NO_HIT();
    }
    double t = 1000;

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    * 
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/

    dvec3 N /* = ... */;

    return Hit(t,N);
}
