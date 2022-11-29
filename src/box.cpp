//
//  Framework for a raytracer
//  File: box.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Author: Maarten Everts
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "box.h"
#include <iostream>
#include <math.h>

Hit Box::intersect(const Ray &ray) const
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
