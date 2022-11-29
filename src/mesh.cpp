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

Hit Mesh::Triangle::intersect(const Ray &ray) const
{
    // test for backface culling: triangle faces away => is invisible
    float proj = ray.D.dot(N);
    if (proj >= 0) {
        return Hit::NO_HIT();
    }

    // project the ray on the plane surface
    float t = (p1 - ray.O).dot(N) / proj;
    Vector target = ray.O + ray.D * t;
    
    // test if hit is inside triangle: the point is on the left side of all
    // oriented edges of the triangle.
    // (oriented counter-clockwise from p1 to p2 to p3 to p1)
    if (
        N.dot((p2 - p1).cross(target - p1)) < 0 ||
        N.dot((p3 - p2).cross(target - p2)) < 0 ||
        N.dot((p1 - p3).cross(target - p3)) < 0
    ) {
        return Hit::NO_HIT();
    }

    // interpolate the vertex normals using barycentric coordinates. it
    // corresponds to the ratio of areas of sub-triangles defined by the target
    // point.
    double a3 = (target - p1).cross(target - p2).length() / 2;
    double a1 = (target - p2).cross(target - p3).length() / 2;
    double a2 = (target - p3).cross(target - p1).length() / 2;
    double area = a1 + a2 + a3;
    double c1 = a1 / area;
    double c2 = a2 / area;
    double c3 = a3 / area;
    Vector norm =(n1 * c1 + n2 * c2 + n3 * c3).normalized();
    // norm = n1;

    // norm = N; // force flat shading
    return Hit(t, norm);
}
