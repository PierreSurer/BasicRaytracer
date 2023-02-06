#include "Box.hpp"
#include <math.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/extended_min_max.hpp>
#include <glm/gtx/component_wise.hpp>

using namespace glm;

Box::Box()
{ }

static const std::vector<ivec2> UV_LOOKUP{
    ivec2{ 0, 0 },
};

std::unique_ptr<BaseHit> Box::intersect(const Ray &ray) const
{
    dvec3 tMin = (-dvec3(1.0) - ray.O) / ray.D;
    dvec3 tMax = (dvec3(1.0)  - ray.O) / ray.D;
    dvec3 t1 = min(tMin, tMax);
    dvec3 t2 = max(tMin, tMax);
    double tNear = compMax(t1);
    double tFar = compMin(t2);
    if(tNear > tFar || tFar < 0.0) return Hit::NO_HIT();

    dvec3 normal = tNear >= 0 ? -sign(ray.D) : sign(ray.D); // normal faces away from ray direction
    normal *= dvec3(equal(t1, dvec3(tNear))); // bit mask to keep first hit direction

    dvec2 uv(0.0);
    dvec3 tangent(0.0);
    auto hit = ray.at(tNear) * 0.5 + 0.5;
    if (normal.x != 0.0) {
        uv = { hit.y, hit.z };
        tangent = { 0.0, 0.0, 1.0 };
    }
    else if (normal.y != 0.0) {
        uv = { hit.x, hit.z };
        tangent = { 0.1, 0.0, 0.0 };
    }
    else {
        uv = { hit.x, hit.y };
        tangent = { 0.0, 1.0, 0.0 };
    }

    return std::make_unique<Hit>(tNear, HitParams{ this, normal, tangent, uv });
}

AABB Box::computeAABB() const
{
    AABB aabb;
    // TODO
    // dvec3 s0 = orientation * dvec3(size.x / 2.0, size.x / 2.0, size.x / 2.0);
    // dvec3 s1 = orientation * dvec3(size.x / 2.0, size.x / 2.0, -size.x / 2.0);
    // dvec3 s2 = orientation * dvec3(size.x / 2.0, -size.x / 2.0, size.x / 2.0);
    // dvec3 s3 = orientation * dvec3(size.x / 2.0, -size.x / 2.0, -size.x / 2.0);
    // dvec3 s4 = orientation * dvec3(-size.x / 2.0, size.x / 2.0, size.x / 2.0);
    // dvec3 s5 = orientation * dvec3(-size.x / 2.0, size.x / 2.0, -size.x / 2.0);
    // dvec3 s6 = orientation * dvec3(-size.x / 2.0, -size.x / 2.0, size.x / 2.0);
    // dvec3 s7 = orientation * dvec3(-size.x / 2.0, -size.x / 2.0, -size.x / 2.0);

    // aabb.first = position + glm::min(glm::min(s0, s1, s2, s3), glm::min(s4, s5, s6, s7));
    // aabb.second = position + glm::max(glm::max(s0, s1, s2, s3), glm::max(s4, s5, s6, s7));

    return aabb;
}