#include "Sphere.hpp"
#include <math.h>
#include <glm/gtx/norm.hpp>
using namespace glm;

Sphere::Sphere()
{ }

std::unique_ptr<BaseHit> Sphere::intersect(const Ray& ray) const
{
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

    dvec3 normal = ray.at(t);

    dvec3 uv = dvec3(0.0);
    uv.x = atan2(normal.z, -normal.x) / (2 * glm::pi<double>()) + 0.5;
    uv.y = acos(-normal.y) / glm::pi<double>();

    dvec3 tangent = normalize(dvec3(-normal.y, normal.x, 0.0));

    return std::make_unique<Hit>(t, HitParams{ this, normal, tangent, uv });
}

AABB Sphere::computeAABB() const
{
    AABB aabb;
    // TODO
    // aabb.first = position - r;
    // aabb.second = position + r;

    return aabb;
}