#include "Mesh.hpp"
#include <algorithm>
#include <numeric>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/extended_min_max.hpp>

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

AABB Mesh::getAABB() const {
    AABB aabb;
    aabb.first = faces.size() ? faces[0].p1 : dvec3(0.0);
    aabb.second = aabb.first;
    
    for (const auto& face: faces) {
        aabb.first = min(aabb.first, face.p1, face.p2, face.p3);
        aabb.second = max(aabb.second, face.p1, face.p2, face.p3);
    }

    return aabb;
}

// min number of triangles to perform a bvh split
static constexpr const auto BVH_THRESHOLD = 4;

static Mesh::BVH recurse_compute_bvh(const std::vector<AABB>& aabbs, std::vector<size_t> indices, int dim) {
    // compute the global aabb
    AABB aabb = indices.size() ? aabbs[0] : AABB{};
    for (auto i: indices) {
        aabb.first = min(aabb.first, aabbs[i].first);
        aabb.second = max(aabb.second, aabbs[i].second);
    }

    // recursion termination if triangle threshold is reach
    if (indices.size() < BVH_THRESHOLD) {
        return Mesh::BVH {
            .aabb = aabb,
            .indices = indices,
            .left = nullptr,
            .right = nullptr,
        };
    }
    
    // sort triangles along an axis
    std::sort(indices.begin(), indices.end(), [&](const auto& a, const auto& b) {
        return aabbs[a].second[dim] < aabbs[b].second[dim];
    });

    // split the set of triangles in half
    int half_index = indices.size() / 2;
    auto id_left = std::vector(indices.begin(), indices.begin() + half_index);
    auto id_right = std::vector(indices.begin() + half_index, indices.end());

    // recursive call
    dim = (dim + 1) % 3;
    return Mesh::BVH {
        .aabb = aabb,
        .indices = indices,
        .left = std::make_unique<Mesh::BVH>(recurse_compute_bvh(aabbs, id_left, dim)),
        .right = std::make_unique<Mesh::BVH>(recurse_compute_bvh(aabbs, id_right, dim))
    };
}

Mesh::BVH Mesh::compute_bvh() const {
    auto aabbs = std::vector<AABB>(faces.size());
    auto indices = std::vector<size_t>(faces.size());

    std::transform(faces.begin(), faces.end(), aabbs.begin(), [](const auto& face) {
        return std::pair(
            min(face.p1, face.p2, face.p3),
            max(face.p1, face.p2, face.p3)
        );
    });

    std::iota(indices.begin(), indices.end(), 0);

    return recurse_compute_bvh(aabbs, indices, 0);
}

void Mesh::optimize() {
    bvh = compute_bvh();
    dirty = false;
}

Mesh::Mesh(std::vector<Triangle> faces)
    : faces(std::move(faces))
{ }

Mesh::Mesh(std::vector<Triangle> faces, const glm::dmat4& mat)
    : Mesh(faces)
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

Hit Mesh::intersect(const Ray& ray) const
{
    if (dirty) {
        throw std::runtime_error("mesh was not optimized");
    }

    // use the acceleration structure
    return recurse_intersect(bvh, ray);

}

Hit Mesh::recurse_intersect(const BVH& bvh, const Ray& ray) const {
    Hit min_hit = Hit::NO_HIT();

    if(intersect_aabb(bvh.aabb, ray)) {
        if (bvh.left != nullptr /* bvh.right is implied */) {
            Hit hit = recurse_intersect(*bvh.left, ray);
            if (hit.t < min_hit.t) {
                min_hit = hit;
            }
            hit = recurse_intersect(*bvh.right, ray);
            if (hit.t < min_hit.t) {
                min_hit = hit;
            }
        }
        else {
            return intersect_indices(bvh.indices, ray);
        }
    }
    return min_hit;
}

Hit Mesh::intersect_indices(const std::vector<size_t> indices, const Ray &ray) const {
    Hit min_hit = Hit::NO_HIT();

    for (auto i : indices) {
        const auto& face = faces[i];
        Hit hit = face.intersect(ray);
        if (hit.t < min_hit.t) {
            min_hit = hit;
        }
    }

    return min_hit;
}

Mesh::Triangle::Triangle(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3, glm::dvec3 n1, glm::dvec3 n2, glm::dvec3 n3)
  : p1(p1), p2(p2), p3(p3), n1(n1), n2(n2), n3(n3)
{
    // compute a main normal
    glm::dvec3 d1 = p2 - p1;
    glm::dvec3 d2 = p3 - p1;
    N = glm::normalize(glm::cross(d1, d2));
}

Mesh::Triangle::Triangle(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3)
    : Triangle(p1, p2, p3, glm::dvec3{}, glm::dvec3{}, glm::dvec3{})
{
    n1 = n2 = n3 = N;
}

void Mesh::Triangle::transform(const dmat4& mat, const dmat4& norm_mat) {
    p1 = mat * dvec4(p1, 1.0);
    p2 = mat * dvec4(p2, 1.0);
    p3 = mat * dvec4(p3, 1.0);
    n1 = norm_mat * dvec4(n1, 1.0);
    n2 = norm_mat * dvec4(n2, 1.0);
    n3 = norm_mat * dvec4(n3, 1.0);

    // compute a main normal
    glm::dvec3 d1 = p2 - p1;
    glm::dvec3 d2 = p3 - p1;
    N = glm::normalize(glm::cross(d1, d2));
}

void Mesh::Triangle::transform(const dmat4& mat) {
    transform(mat, transpose_inverse(mat));
}

Hit Mesh::Triangle::intersect(const Ray &ray) const
{
    // test for backface culling: triangle faces away => is invisible
    double proj = dot(ray.D, N);

    // project the ray on the plane surface
    double t = dot((p1 - ray.O), N) / proj;
    dvec3 target = ray.O + ray.D * t;
    
    // test if hit is inside triangle: the point is on the left side of all
    // oriented edges of the triangle.
    // (oriented counter-clockwise from p1 to p2 to p3 to p1)
    if (
        t < 0.0 ||
        dot(N, cross((p2 - p1), (target - p1))) < 0.0 ||
        dot(N, cross((p3 - p2), (target - p2))) < 0.0 ||
        dot(N, cross((p1 - p3), (target - p3))) < 0.0
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

    // uncomment this for flat shading
    // norm = N;

    return Hit(t, norm);
}
