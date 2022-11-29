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
#include <vector>

class Mesh : public Object
{
public:
    class Triangle;

    Mesh(std::vector<Triangle> faces)
        : faces(faces)
    { }

    Hit intersect(const Ray &ray) const;


    class Triangle: public Object
    {
    public:
        Triangle(Point p1, Point p2, Point p3, Vector n1, Vector n2, Vector n3)
            : p1(p1), p2(p2), p3(p3), n1(n1), n2(n2), n3(n3)
        {
            // compute a main normal
            Point d1 = p2 - p1;
            Point d2 = p3 - p1;
            N = d1.cross(d2).normalized();
        }
        Triangle(Point p1, Point p2, Point p3)
            : Triangle(p1, p2, p3, Vector{}, Vector{}, Vector{})
        {
            n1 = n2 = n3 = N;
        }

        Hit intersect(const Ray &ray) const override;

        Point p1, p2, p3;
        Vector n1, n2, n3;
        Vector N;
    };

private:
    std::vector<Triangle> faces;
};
