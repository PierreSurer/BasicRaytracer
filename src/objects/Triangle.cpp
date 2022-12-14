#include "Triangle.hpp"
#include <math.h>

Hit Triangle::intersect(const Ray &ray) const
{
    // compute a normal
    glm::dvec3 d1 = p2 - p1;
    glm::dvec3 d2 = p3 - p1;
    glm::dvec3 N = normalize(cross(d1, d2));

    double proj = dot(ray.D, N);
    if (proj > 0.0) { // triangle faces away => is invisible
        return Hit::NO_HIT();
    }
    
    // project the ray on the plane surface
    double t = dot((p1 - ray.O), N) / proj;
    glm::dvec3 target = ray.at(t);
    
    // test if hit is inside triangle: the point is on the left side of all
    // oriented edges of the triangle.
    // (oriented counter-clockwise from p1 to p2 to p3 to p1)
    if (
        t < 0.0 ||
        dot(N, cross(p2 - p1, target - p1)) < 0.0 ||
        dot(N, cross(p3 - p2, target - p2)) < 0.0 ||
        dot(N, cross(p1 - p3, target - p3)) < 0.0
    ) {
        return Hit::NO_HIT();
    }

    return Hit(t, N);
}
