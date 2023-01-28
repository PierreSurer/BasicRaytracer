#include "Sphere.hpp"
#include <math.h>
#include <glm/gtx/norm.hpp>
using namespace glm;

Sphere::Sphere(dmat4 model)
{
    setModel(model);
    velocity = glm::dvec3(40.0, 0.0, 0.0);
}

std::unique_ptr<BaseHit> Sphere::intersect(const Ray& globalRay) const
{
    Ray ray = computeLocalRay(globalRay);

    // dvec3 pos = velocity * ray.time;
    dvec3 OC = -ray.O;

    double t1 = dot(OC, ray.D);
    double h_2 = length2(OC) - t1 * t1;
    if (h_2 > 1.0) {
        return Hit::NO_HIT();
    }

    double t;
    if(length2(OC) >= 1.0)
        t = t1 - sqrt(1.0 - h_2);
    else
        t = t1 + sqrt(1.0 - h_2);

    if (t < 0.0) {
        return Hit::NO_HIT();
    }

    dvec3 intersectionPoint = ray.at(t);
    dvec3 normal = normalize(normModel * intersectionPoint);
    dvec3 uv = dvec3(0.0); // TODO

    // transform local time back to global space
    t *= length(dmat3(model) * ray.D);

    return std::make_unique<Hit>(t, HitParams{ this, normal, uv });
}

AABB Sphere::computeAABB() const {
    AABB aabb;
    // TODO
    // aabb.first = position - r;
    // aabb.second = position + r;

    return aabb;
}