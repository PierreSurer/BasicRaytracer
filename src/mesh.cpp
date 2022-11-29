//
//  Framework for a raytracer
//  File: mesh.cpp
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

#include "mesh.h"
#include <algorithm>

Hit Mesh::intersect(const Ray& ray) const
{
    for (const Triangle& face : faces) {
        Hit hit = face.intersect(ray);
        if (!hit.no_hit) {
            return hit;
        }
    }

    return Hit::NO_HIT();
}
