#include "Triangle.hpp"
#include <math.h>
#include <glm/gtx/extended_min_max.hpp>

Triangle::Triangle(glm::dvec3 v1, glm::dvec3 v2, glm::dvec3 v3)
    : Triangle(v1, v2, v3, glm::dvec3{}, glm::dvec3{}, glm::dvec3{})
{
    // compute a main normal
    glm::dvec3 d1 = v2 - v1;
    glm::dvec3 d2 = v3 - v1;
    n1 = n2 = n3 = glm::normalize(glm::cross(d1, d2));
}

Triangle::Triangle(glm::dvec3 v1, glm::dvec3 v2, glm::dvec3 v3, glm::dvec3 n1, glm::dvec3 n2, glm::dvec3 n3) 
    : v1(v1), v2(v2), v3(v3), n1(n1), n2(n2), n3(n3)
{

}


Hit Triangle::intersect(const Ray &ray) const
{
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection.html
    glm::dvec3 v1v2 = v2 - v1;
    glm::dvec3 v1v3 = v3 - v1;
    glm::dvec3 pvec = cross(ray.D, v1v3);
    double det = dot(v1v2, pvec);
    // ray and triangle are parallel if det is close to 0
    // if (fabs(det) < kEpsilon) return false;
    double invDet = 1.0 / det;

    glm::dvec3 tvec = ray.O - v1;
    double u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0) return Hit::NO_HIT();

    glm::dvec3 qvec = cross(tvec, v1v2);
    double v = dot(ray.D, qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) return Hit::NO_HIT();
    
    double t = dot(v1v3, qvec) * invDet;
    
    double w = 1.0 - u - v;

    return Hit(t, u * n1 + v * n2 + w * n3);
    
}

AABB Triangle::getAABB() const {
    AABB aabb;
    aabb.first = glm::min(v1, v2, v3);
    aabb.second = glm::max(v1, v2, v3);

    return aabb;
}