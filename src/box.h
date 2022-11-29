//
//  Framework for a raytracer
//  File: box.h
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

class Box : public Object
{
public:
    Box(Point position, double size) : position(position), size(size) { }

    Hit intersect(const Ray &ray) const override;

    const Point position;
    const double size;
};
