#include "Cylinder.hpp"
#include <math.h>
#include <glm/gtx/quaternion.hpp>


using namespace glm;

Cylinder::Cylinder()
    : height(1.0), radius(1.0)
{
}

std::unique_ptr<BaseHit> Cylinder::intersect(const Ray &ray) const
{
    
    double a = (ray.D.x * ray.D.x) + (ray.D.z * ray.D.z);
    double b = 2.0 * (ray.D.x * ray.O.x + ray.D.z * ray.O.z);
    double c = ray.O.x * ray.O.x + ray.O.z * ray.O.z - radius * radius;
    
    double delta = b * b - 4.0 * a * c;
    if(delta < 0.0) return Hit::NO_HIT();
    
    double t1 = (-b - sqrt(delta)) / (2.0 * a);
    double t2 = (-b + sqrt(delta)) / (2.0 * a);
    if(t1 > t2) std::swap(t1, t2);
    
    double r = ray.O.y + t1 * ray.D.y;
    if (abs(r) <= height) { //circle intersection
        if (t2 < 0.0)
            return Hit::NO_HIT();
        else
            if(t1 > 0.0) {//outside cylinder
                dvec3 pos = ray.O + t1 * ray.D;
                dvec3 N = normalize(dvec3(pos.x, 0.0, pos.z));
                dvec2 uv = dvec2(atan2(N.z, -N.x) / (2 * glm::pi<double>()) + 0.5, 0.5 + r / (4.0 * height));
                return std::make_unique<Hit>(t1, HitParams{ this, N, uv });
            }
            else {
                dvec3 pos = ray.O + t2 * ray.D;
                dvec3 N = normalize(dvec3(-pos.x, 0.0, -pos.z));
                dvec2 uv = dvec2(atan2(N.z, -N.x) / (2 * glm::pi<double>()) + 0.5, 0.5 + r / (4.0 * height)); // TODO
                return std::make_unique<Hit>(t1, HitParams{ this, N, uv });
            }
            
    }
    else {
        dvec3 N = normalize(dvec3(0.0, -ray.D.y, 0.0));
        double proj = dot(ray.D, N);
        
        // project the ray on the plane surface
        double t = dot(dvec3(0.0, height * N.y, 0.0) - ray.O, N) / proj;
        dvec3 pos = ray.O + t * ray.D;
        if(t < 0.0 || pos.x * pos.x + pos.z * pos.z > radius * radius) 
            return Hit::NO_HIT();
        
        dvec2 uv = dvec2(0.5 + pos.x / (2.0 * radius), 0.5 + pos.z / (2.0 * radius)); // TODO
        return std::make_unique<Hit>(t1, HitParams{ this, N, uv });
    }
}

AABB Cylinder::computeAABB() const {
    AABB aabb;
    return aabb;
}