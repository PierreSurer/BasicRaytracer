//
//  Framework for a raytracer
//  File: triangle.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "triangle.h"
#include <iostream>
#include <math.h>

/************************** Triangle **********************************/

Hit Triangle::intersect(const Ray &ray)
{
    // compute a normal
    Point d1 = p2 - p1;
    Point d2 = p3 - p1;
    Vector N = d1.cross(d2).normalized();

    float proj = ray.D.dot(N);
    if (proj > 0) { // triangle faces away => is invisible
        std::cout << "hello" << std::endl;
        return Hit::NO_HIT();
    }
    
    // project the ray on the plane surface
    float t = (p1 - ray.D).dot(N) / proj;
    Vector target = ray.O + ray.D * t;
    
    return Hit(t, N);
    
    // test if hit is inside triangle : the point is on the left side
    // of all oriented edges of the triangle.
    // (oriented clockwise from p1 to p2 to p3 to p1)
    
    if (
        (p2 - p1).dot(target - p1) > 0 &&
        (p3 - p2).dot(target - p2) > 0 &&
        (p1 - p3).dot(target - p3) > 0
    ) {
        return Hit(t, N);
    }

    return Hit::NO_HIT();
}
