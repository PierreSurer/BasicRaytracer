#pragma once

#include "Object.hpp"
#include "Triangle.hpp"
#include "Box.hpp"

#include <glm.hpp>
#include <vector>

class Mesh : public Object
{
public:
    Mesh(std::vector<Triangle> faces);
    Mesh(std::vector<Triangle> faces, const glm::dmat4& mat);

    // apply a transform matrix to the vertices and normals
    void transform(const glm::dmat4& mat);
    void transform(const glm::dmat4& mat, const glm::dmat4& norm_mat);

    // recompute the acceleration structure
    void optimize();

    std::unique_ptr<BaseHit> intersect(const Ray &ray) const override;

private:
    std::vector<Triangle> faces;

protected:
    AABB computeAABB() const override;

// acceleration structures
public:
    struct BVH {
        AABB aabb;
        std::vector<size_t> indices;
        std::unique_ptr<BVH> left, right;
    };
    
private:
    BVH compute_bvh() const;
    std::unique_ptr<BaseHit> recurse_intersect(const BVH& bvh, const Ray &ray) const;
    std::unique_ptr<BaseHit> intersect_indices(const std::vector<size_t> indices, const Ray &ray) const;

    // (marked mutable as this is opaque from outsider's perspective)
    bool dirty = true;
    BVH bvh;
};
