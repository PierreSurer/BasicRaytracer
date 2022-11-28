#include "box.h"
#include <iostream>
#include <math.h>

/************************** Sphere **********************************/

Hit Box::intersect(const Ray &ray)
{
    Vector tMin = ((position - size) - ray.O) / ray.D;
    Vector tMax = ((position + size) - ray.O) / ray.D;
    Vector t1;
    Vector t2;
    t1.x = min(tMin.x, tMax.x);
    t1.y = min(tMin.y, tMax.y);
    t1.z = min(tMin.z, tMax.z);
    t2.x = max(tMin.x, tMax.x);
    t2.y = max(tMin.y, tMax.y);
    t2.z = max(tMin.z, tMax.z);
    double tNear = max(max(t1.x, t1.y), t1.z);
    double tFar = min(min(t2.x, t2.y), t2.z);
    if(tNear > tFar) return Hit::NO_HIT();
    
    Vector intersectionPoint = ray.O + tNear * ray.D;
    Vector N = (intersectionPoint - position).normalized();

    return Hit(tNear,N);
}
