#include "Sphere.hpp"
#include <math.h>
#include <glm/gtx/norm.hpp>
using namespace glm;

Hit Sphere::intersect(const Ray &ray) const
{
    glm::dvec3 OC = position - ray.O;
    // Min distance between ray and object position
    double h_2 = length2(cross(ray.D, OC));
    double r_2 = length2(r);
    if (h_2 > r_2) {
        return Hit::NO_HIT();
    }
    double t = dot(OC, ray.D) - sqrt(r_2 - h_2);
    if (t < 0.0) {
        return Hit::NO_HIT();
    }

    glm::dvec3 intersectionPoint = ray.at(t);
    glm::dvec3 N = (intersectionPoint - position) / r;

    return Hit(t, N);
}
