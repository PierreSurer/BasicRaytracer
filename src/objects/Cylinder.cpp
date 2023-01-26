#include "Cylinder.hpp"
#include <math.h>
#include <glm/gtx/quaternion.hpp>


using namespace glm;

Cylinder::Cylinder(dvec3 position, dvec3 rotation, double height, double radius)
    : position(position), rotation(rotation), height(height), radius(radius)
{
    dquat rot = dquat(rotation);
    orientation = mat3_cast(rot);
    inv_orientation = inverse(orientation);
}

std::unique_ptr<BaseHit> Cylinder::intersect(const Ray &ray) const
{
    dvec3 localOrigin = inv_orientation * (ray.O - position);
    dvec3 localDirection = inv_orientation * ray.D;

    double a = (localDirection.x * localDirection.x) + (localDirection.z * localDirection.z);
    double b = 2.0 * (localDirection.x * localOrigin.x + localDirection.z * localOrigin.z);
    double c = localOrigin.x * localOrigin.x + localOrigin.z * localOrigin.z - radius * radius;
    
    double delta = b * b - 4.0 * a * c;
    if(delta < 0.0) return Hit::NO_HIT();
    
    double t1 = (-b - sqrt(delta)) / (2.0 * a);
    double t2 = (-b + sqrt(delta)) / (2.0 * a);
    if(t1 > t2) std::swap(t1, t2);
    
    double r = localOrigin.y + t1 * localDirection.y;
    if (abs(r) <= height) { //circle intersection
        if (t2 < 0.0)
            return Hit::NO_HIT();
        else
            if(t1 > 0.0) {//outside cylinder
                dvec3 pos = localOrigin + t1 * localDirection;
                dvec3 N = normalize(dvec3(pos.x, 0.0, pos.z));
                dvec3 uv = dvec3(0.0); // TODO
                return std::make_unique<Hit>(t1, HitParams{ this, N, uv });
            }
            else {
                dvec3 pos = localOrigin + t2 * localDirection;
                dvec3 N = normalize(dvec3(-pos.x, 0.0, -pos.z));
                dvec3 uv = dvec3(0.0); // TODO
                return std::make_unique<Hit>(t1, HitParams{ this, N, uv });
            }
            
    }
    else {
        return Hit::NO_HIT();
        dvec3 N = normalize(dvec3(0.0, -localDirection.y, 0.0));
        double proj = dot(localDirection, N);
        
        // project the ray on the plane surface
        double t = dot(dvec3(0.0, height * N.y, 0.0) - localOrigin, N) / proj;
        dvec3 pos = localOrigin + t * localDirection;
        if(t < 0.0 || pos.x * pos.x + pos.z * pos.z > radius * radius) 
            return Hit::NO_HIT();
        
        dvec3 uv = dvec3(0.0); // TODO
        return std::make_unique<Hit>(t1, HitParams{ this, N, uv });
    }
}

AABB Cylinder::computeAABB() const {
    AABB aabb;
    return aabb;
}