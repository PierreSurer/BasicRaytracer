#include "Mesh.hpp"
#include "Triangle.hpp"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/extended_min_max.hpp>
#include <glm/gtx/associated_min_max.hpp>


using namespace glm;

using AABB = std::pair<glm::dvec3, glm::dvec3>;

static dmat4 transpose_inverse(const dmat4& mat) {
    return transpose(inverse(mat));
}

static bool intersect_aabb(const AABB& aabb, const Ray& ray) {
    // test ray is inside aabb
    if (all(lessThan(aabb.first, ray.O) & lessThan(ray.O, aabb.second))) {
        return true;
    }

    // test regular intersection
    dvec3 tMin = (aabb.first - ray.O) / ray.D;
    dvec3 tMax = (aabb.second - ray.O) / ray.D;
    dvec3 t1 = min(tMin, tMax);
    dvec3 t2 = max(tMin, tMax);
    double tNear = compMax(t1);
    double tFar = compMin(t2);
    return (tNear <= tFar && tNear >= 0.0);
}

AABB Mesh::computeAABB() const {
    AABB aabb;
    aabb.first = faces.size() ? faces[0].v1 : dvec3(0.0);
    aabb.second = aabb.first;
    
    for (const auto& face: faces) {
        aabb.first = min(aabb.first, face.v1, face.v2, face.v3);
        aabb.second = max(aabb.second, face.v1, face.v2, face.v3);
    }

    return aabb;
}

// min number of triangles to perform a bvh split
static constexpr const auto BVH_THRESHOLD = 32; // value empirically measured as optimal

static Mesh::BVH recurse_compute_bvh(const std::vector<AABB>& aabbs, std::vector<size_t> indices) {
    // compute the global aabb
    AABB aabb = indices.size() ? aabbs[indices[0]] : AABB{};
    for (auto i : indices) {
        aabb.first = min(aabb.first, aabbs[i].first);
        aabb.second = max(aabb.second, aabbs[i].second);
    }

    // recursion termination if triangle threshold is reach
    if (indices.size() < BVH_THRESHOLD) {
        return Mesh::BVH {
            aabb,
            indices,
            nullptr, // left
            nullptr, // right
        };
    }

    // best dimension to perform the split
    auto size = aabb.second - aabb.first;
    auto dim = associatedMax(size.x, 0, size.y, 1, size.z, 2);
    
    // sort triangles along an axis
    std::sort(indices.begin(), indices.end(), [&](const auto& a, const auto& b) {
        auto ca = (aabbs[a].first[dim] + aabbs[a].second[dim]) / 2;
        auto cb = (aabbs[b].first[dim] + aabbs[b].second[dim]) / 2;
        return ca < cb;
    });

    // split the set of triangles in two partitions (method 1 or 2)
    decltype(indices.begin()) half_index;

    // method 1. partition by equal element count
    {
        half_index = indices.begin() + indices.size() / 2;
    }

    // method 2. partition by equal space size
    {
        auto center = (aabb.first[dim] + aabb.second[dim]) / 2;
        half_index = std::find_if(indices.begin(), indices.end(), [&](const auto& i) {
            auto tri_center = (aabbs[i].first[dim] + aabbs[i].second[dim]) / 2;
            return tri_center >= center;
        });

        if (half_index == indices.begin() || half_index == indices.end()) {
            half_index = indices.begin() + indices.size() / 2;
        }
    }

    auto id_left = std::vector(indices.begin(), half_index);
    auto id_right = std::vector(half_index, indices.end());

    // recursive call
    return Mesh::BVH {
        aabb,
        indices,
        std::make_unique<Mesh::BVH>(recurse_compute_bvh(aabbs, id_left)),
        std::make_unique<Mesh::BVH>(recurse_compute_bvh(aabbs, id_right))
    };
}

Mesh::BVH Mesh::compute_bvh() const {
    auto aabbs = std::vector<AABB>(faces.size());
    auto indices = std::vector<size_t>(faces.size());

    std::transform(faces.begin(), faces.end(), aabbs.begin(), [](const auto& face) {
        return std::pair(
            min(face.v1, face.v2, face.v3),
            max(face.v1, face.v2, face.v3)
        );
    });

    std::iota(indices.begin(), indices.end(), 0);

    return recurse_compute_bvh(aabbs, indices);
}

void Mesh::optimize() {
    bvh = compute_bvh();
    dirty = false;
}

Mesh::Mesh(std::vector<Triangle> faces)
    : faces(std::move(faces))
{ }

Mesh::Mesh(std::vector<Triangle> faces, const glm::dmat4& mat)
    : Mesh(std::move(faces))
{
    transform(mat);
}

void Mesh::transform(const dmat4& mat) {
    transform(mat, transpose_inverse(mat));
    dirty = true;
}

void Mesh::transform(const dmat4& mat, const dmat4& norm_mat) {
    std::for_each(begin(faces), end(faces), [&](auto& f) {
        f.transform(mat, norm_mat);
    });
    dirty = true;
}

std::unique_ptr<BaseHit> Mesh::intersect(const Ray& ray) const
{
    if (dirty) {
        throw std::runtime_error("mesh was not optimized");
    }

    // use the acceleration structure
    return recurse_intersect(bvh, ray);

    // just use the aabb
    // Hit min_hit = Hit::NO_HIT();

    // if (!intersect_aabb(bvh.aabb, ray)) {
    //     return min_hit;
    // }

    // for (const auto& face : faces) {
    //     Hit hit = face.intersect(ray);
    //     if (hit.t < min_hit.t) {
    //         min_hit = hit;
    //     }
    // }

    // return min_hit;
}

std::unique_ptr<BaseHit> Mesh::recurse_intersect(const BVH& bvh, const Ray& ray) const {
    auto min_hit = Hit::NO_HIT();

    if(intersect_aabb(bvh.aabb, ray)) {
        if (bvh.left != nullptr /* bvh.right is implied */) {
            auto hit = recurse_intersect(*bvh.left, ray);
            if (hit->t < min_hit->t) {
                min_hit = std::move(hit);
            }
            hit = recurse_intersect(*bvh.right, ray);
            if (hit->t < min_hit->t) {
                min_hit = std::move(hit);
            }
        }
        else {
            return intersect_indices(bvh.indices, ray);
        }
    }
    return min_hit;
}

std::unique_ptr<BaseHit> Mesh::intersect_indices(const std::vector<size_t> indices, const Ray &ray) const {
    auto min_hit = Hit::NO_HIT();

    for (auto i : indices) {
        const auto& face = faces[i];
        auto hit = face.intersect(ray);
        if (hit->t < min_hit->t) {
            min_hit = std::move(hit);
        }
    }

    return min_hit;
}
