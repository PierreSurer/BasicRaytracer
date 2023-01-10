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

#include "Object.hpp"
#include "objects/Box.hpp"
#include <glm.hpp>
#include <vector>

class Mesh : public Object
{
public:
    class Triangle;

    Mesh(std::vector<Triangle> faces);

    Mesh(std::vector<Triangle> faces, const glm::dmat4& mat);

    void transform(const glm::dmat4& mat);
    void transform(const glm::dmat4& mat, const glm::dmat4& norm_mat);

    Hit intersect(const Ray &ray) const;


    class Triangle: public Object
    {
    public:
        Triangle(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3, glm::dvec3 n1, glm::dvec3 n2, glm::dvec3 n3)
            : p1(p1), p2(p2), p3(p3), n1(n1), n2(n2), n3(n3)
        {
            // compute a main normal
            glm::dvec3 d1 = p2 - p1;
            glm::dvec3 d2 = p3 - p1;
            N = glm::normalize(glm::cross(d1, d2));
        }
        Triangle(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3)
            : Triangle(p1, p2, p3, glm::dvec3{}, glm::dvec3{}, glm::dvec3{})
        {
            n1 = n2 = n3 = N;
        }

        void transform(const glm::dmat4& mat);
        void transform(const glm::dmat4& mat, const glm::dmat4& norm_mat);

        Hit intersect(const Ray &ray) const override;

        glm::dvec3 p1, p2, p3;
        glm::dvec3 n1, n2, n3;
        glm::dvec3 N;
    };

private:
    void compute_aabb();

    std::vector<Triangle> faces;
    Box aabb;
};
