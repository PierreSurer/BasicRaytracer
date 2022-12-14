#include "Cylinder.hpp"
#include <math.h>
#include <glm/gtx/quaternion.hpp>

Hit Cylinder::intersect(const Ray &ray) const
{
    glm::dquat rot = glm::dquat(rotation);
    glm::dmat3 orientation = glm::mat3_cast(rot);
    glm::dvec3 localOrigin = glm::inverse(orientation) * (ray.O - position);
    glm::dvec3 localDirection = glm::inverse(orientation) * ray.D;

    double a = (localDirection.x * localDirection.x) + (localDirection.z * localDirection.z);
    double b = 2.0 * (localDirection.x * localOrigin.x + localDirection.z * localOrigin.z);
    double c = localOrigin.x * localOrigin.x + localOrigin.z * localOrigin.z - radius * radius;
    
    double delta = b * b - 4.0 * a * c;
    if(delta < 0.0) return Hit::NO_HIT();
    
    double t1 = (-b - sqrt(delta)) / (2.0 * a);
    double t2 = (-b + sqrt(delta)) / (2.0 * a);
    double t = std::min(t1, t2);
    
    double r = localOrigin.y + t * localDirection.y;
    if (abs(r) <= height) {
        glm::dvec3 pos = localOrigin + t * localDirection;
        glm::dvec3 N = normalize(glm::dvec3(pos.x, 0.0, pos.z));
        if (t < 0.0)
            return Hit::NO_HIT();
        else
            return Hit(t, orientation * N);
    }
    else {
        glm::dvec3 N = normalize(glm::dvec3(0.0, 1.0, 0.0));
        if(localOrigin.y < 0.0) N = -N;
        double proj = dot(localDirection, N);
        if (proj > 0.0) { // faces away => is invisible
            return Hit::NO_HIT();
        }
        
        // project the ray on the plane surface
        double t = dot(glm::dvec3(0.0, height * N.y, 0.0) - localOrigin, N) / proj;
        glm::dvec3 pos = localOrigin + t * localDirection;
        if(t < 0.0 || pos.x * pos.x + pos.z * pos.z > radius * radius) 
            return Hit::NO_HIT();
        
        return Hit(t, orientation * N);
    }
}
