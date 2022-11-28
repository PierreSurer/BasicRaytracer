//
//  Framework for a raytracer
//  File: triangle.h
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

#pragma once

#include "object.h"

class Triangle : public Object
{
public:
    Triangle(Point p1, Point p2, Point p3) : p1(p1), p2(p2), p3(p3) { }

    virtual Hit intersect(const Ray &ray);

    // p1, p2, p3 must be in clockwise order when looking at the visible side.
    const Point p1, p2, p3;
};
