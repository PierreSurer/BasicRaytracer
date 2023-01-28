#include "Box.hpp"
#include <math.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/extended_min_max.hpp>
#include <glm/gtx/component_wise.hpp>

using namespace glm;

Box::Box(dmat4 model)
{
    setModel(model);
}

std::unique_ptr<BaseHit> Box::intersect(const Ray &globalRay) const
{
    Ray ray = computeLocalRay(globalRay);

    dvec3 tMin = (-dvec3(1.0) - ray.O) / ray.D;
    dvec3 tMax = (dvec3(1.0)  - ray.O) / ray.D;
    dvec3 t1 = min(tMin, tMax);
    dvec3 t2 = max(tMin, tMax);
    double tNear = compMax(t1);
    double tFar = compMin(t2);
    if(tNear > tFar || tFar < 0.0) return Hit::NO_HIT();
    
    dvec3 normal;

    // TODO: can be optimized
    // dvec3 N = sign(ray.D) * dvec3(equal(t1, dvec3(tNear))) * (tNear < 0 ? 1.0 : -1.0);

    if(tNear >= 0.0) { // if outside box
        if(t1.x == tNear)      normal = {-sign(ray.D.x), 0.0, 0.0};
        else if(t1.y == tNear) normal = {0.0, -sign(ray.D.y), 0.0};
        else                   normal = {0.0, 0.0, -sign(ray.D.z)};
    } else {
        if(t2.x == tFar)       normal = {sign(ray.D.x), 0.0, 0.0};
        else if(t2.y == tFar)  normal = {0.0, sign(ray.D.y), 0.0};
        else                   normal = {0.0, 0.0, sign(ray.D.z)};
    }

    normal = normalize(normModel * normal);

    // TODO uv
    dvec2 uv(0.0);

    // transform local time back to global space
    tNear *= length(model * vec4(ray.D, 0.0));

    return std::make_unique<Hit>(tNear, HitParams{ this, normal, uv });
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