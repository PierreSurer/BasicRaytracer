#include "Sphere.hpp"
#include <math.h>

Hit Sphere::intersect(const Ray &ray) const
{
    glm::dvec3 OC = position - ray.O;
    // Min distance between ray and object position
    double h = length(cross(ray.D, OC));
    if (h > r) {
        return Hit::NO_HIT();
    }
    double t = dot(OC, ray.D) - sqrt(r * r - h * h);
    if (t < 0.0) {
        return Hit::NO_HIT();
    }

    glm::dvec3 intersectionPoint = ray.at(t);
    glm::dvec3 N = normalize(intersectionPoint - position);

    return Hit(t, N);
}
