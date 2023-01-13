#pragma once

#include "Object.hpp"

class Triangle : public Object
{
public:
    // flat shaded
    Triangle(glm::dvec3 v1, glm::dvec3 v2, glm::dvec3 v3);
    // custom normals per triangle (e.g. smooth shaded)
    Triangle(glm::dvec3 v1, glm::dvec3 v2, glm::dvec3 v3, glm::dvec3 n1, glm::dvec3 n2, glm::dvec3 n3);

    Hit intersect(const Ray &ray) const override;

    // apply a transform matrix to the vertices and normals
    void transform(const glm::dmat4& mat);
    void transform(const glm::dmat4& mat, const glm::dmat4& norm_mat);

protected:
    AABB computeAABB() const override;

private:
    friend class Mesh;
    glm::dvec3 v1, v2, v3;
    glm::dvec3 n1, n2, n3, N;
};
