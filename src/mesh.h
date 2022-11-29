//
//  Framework for a raytracer
//  File: mesh.h
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
#include "triangle.h"
#include <vector>

class Mesh : public Object
{
public:
    Mesh(std::vector<Triangle> faces)
        : faces(faces)
    { }

    Hit intersect(const Ray &ray) const;

    std::vector<Triangle> faces;
};
