//
//  Framework for a raytracer
//  File: sphere.cpp
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

#include "sphere.h"
#include <iostream>
#include <math.h>

Hit Sphere::intersect(const Ray &ray) const
{
    // compute intersection
    Vector OC = position - ray.O;
    double h = ray.D.cross(OC).length();
    if (h > r) {
        return Hit::NO_HIT();
    }
    double t = OC.dot(ray.D) - sqrt(r * r - h * h);

    // compute normal
    Vector intersectionPoint = ray.O + t * ray.D;
    Vector N = (intersectionPoint - position).normalized();

    return Hit(t, N);
}
