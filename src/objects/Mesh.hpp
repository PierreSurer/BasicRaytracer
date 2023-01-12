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

    // apply a transform matrix to the vertices and normals
    void transform(const glm::dmat4& mat);
    void transform(const glm::dmat4& mat, const glm::dmat4& norm_mat);

    // recompute the acceleration structure
    void optimize();

    Hit intersect(const Ray &ray) const;

private:
    std::vector<Triangle> faces;

// acceleration structures
public:
    using AABB = std::pair<glm::dvec3, glm::dvec3>;
    struct BVH {
        AABB aabb;
        std::vector<size_t> indices;
        std::unique_ptr<BVH> left, right;
    };
    
private:
    AABB compute_aabb() const;
    BVH compute_bvh() const;
    Hit recurse_intersect(const BVH& bvh, const Ray &ray) const;
    Hit intersect_indices(const std::vector<size_t> indices, const Ray &ray) const;

    // (marked mutable as this is opaque from outsider's perspective)
    bool dirty = true;
    BVH bvh;
};

class Mesh::Triangle: public Object
{
public:
    // flat shaded
    Triangle(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3);
    // custom normals per triangle (e.g. smooth shaded)
    Triangle(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3, glm::dvec3 n1, glm::dvec3 n2, glm::dvec3 n3);

    // apply a transform matrix to the vertices and normals
    void transform(const glm::dmat4& mat);
    void transform(const glm::dmat4& mat, const glm::dmat4& norm_mat);

    Hit intersect(const Ray &ray) const override;

    glm::dvec3 p1, p2, p3;
    glm::dvec3 n1, n2, n3;
    glm::dvec3 N;
};

