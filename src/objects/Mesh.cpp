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

#include "Mesh.hpp"
#include <algorithm>

using namespace glm;

Hit Mesh::intersect(const Ray& ray) const
{
    Hit min_hit = Hit::NO_HIT();
    for (const auto &face : faces) {
        Hit hit = face.intersect(ray);
        if (hit.t < min_hit.t) {
            min_hit = hit;
        }
    }

    return min_hit;
}

Hit Mesh::Triangle::intersect(const Ray &ray) const
{
    // test for backface culling: triangle faces away => is invisible
    double proj = dot(ray.D, N);
    if (proj >= 0) {
        return Hit::NO_HIT();
    }

    // project the ray on the plane surface
    double t = dot((p1 - ray.O), N) / proj;
    dvec3 target = ray.O + ray.D * t;
    
    // test if hit is inside triangle: the point is on the left side of all
    // oriented edges of the triangle.
    // (oriented counter-clockwise from p1 to p2 to p3 to p1)
    if (
        dot(N, cross((p2 - p1), (target - p1))) < 0 ||
        dot(N, cross((p3 - p2), (target - p2))) < 0 ||
        dot(N, cross((p1 - p3), (target - p3))) < 0
    ) {
        return Hit::NO_HIT();
    }

    // interpolate the vertex normals using barycentric coordinates. it
    // corresponds to the ratio of areas of sub-triangles defined by the target
    // point.
    double a3 = length(cross((target - p1), (target - p2))) / 2.0;
    double a1 = length(cross((target - p2), (target - p3))) / 2.0;
    double a2 = length(cross((target - p3), (target - p1))) / 2.0;
    double area = a1 + a2 + a3;
    double c1 = a1 / area;
    double c2 = a2 / area;
    double c3 = a3 / area;
    dvec3 norm = normalize(n1 * c1 + n2 * c2 + n3 * c3);

    return Hit(t, norm);
}
