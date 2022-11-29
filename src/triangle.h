//
//  Framework for a raytracer
//  File: triangle.h
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

#pragma once

#include "object.h"

class Triangle : public Object
{
public:
    Triangle(Point p1, Point p2, Point p3)
        : p1(p1), p2(p2), p3(p3)
    { }

    Hit intersect(const Ray &ray) const override;

    // p1, p2, p3 must be in counter-clockwise order when looking at the visible side.
    const Point p1, p2, p3;
};
