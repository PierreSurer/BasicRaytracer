#include "Sphere.hpp"
#include <math.h>
#include <glm/gtx/norm.hpp>
using namespace glm;

Sphere::Sphere(dvec3 position, double r, dvec3 rotation)
    : position(position), rotation(rotation), r(r)
{
    setVelocity(glm::dvec3(0.0, 0.0, 0.0));

 }

std::unique_ptr<BaseHit> Sphere::intersect(const Ray &ray) const
{
    dvec3 pos = position + velocity * ray.delay;
    dvec3 OC = pos - ray.O;
    // Min distance between ray and object position
    double h_2 = length2(cross(ray.D, OC));
    double r_2 = length2(r);
    if (h_2 > r_2) {
        return Hit::NO_HIT();
    }
    double t;
    if(length2(OC) >= r_2)
        t = dot(OC, ray.D) - sqrt(r_2 - h_2);
    else
        t = dot(OC, ray.D) + sqrt(r_2 - h_2);
    
    if (t < 0.0) {
        return Hit::NO_HIT();
    }

    dvec3 intersectionPoint = ray.at(t);
    dvec3 N = (intersectionPoint - pos) / r;

    dquat rot = dquat({rotation.x, rotation.y, -rotation.z});
    dvec3 uvN = mat3_cast(rot) * N;
    dvec2 uv = dvec2(0.0);

    uv.x = atan2(uvN.z, -uvN.x) / (2 * glm::pi<double>()) + 0.5;
    uv.y = acos(-uvN.y) / glm::pi<double>();

    return std::make_unique<Hit>(t, HitParams{ this, N, uv });
}

AABB Sphere::computeAABB() const {
    AABB aabb;
    aabb.first = position - r;
    aabb.second = position + r;

    return aabb;
}