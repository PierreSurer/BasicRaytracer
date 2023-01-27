#include "Triangle.hpp"
#include <math.h>
#include <glm/gtx/extended_min_max.hpp>

using namespace glm;

static dmat4 transpose_inverse(const dmat4& mat) {
    return transpose(inverse(mat));
}


class TriangleHit : public BaseHit {
public:
    TriangleHit(const Triangle* tri, Ray ray, double t);

    HitParams params() const override;

private:
    Ray ray;
    const Triangle* tri;
};

TriangleHit::TriangleHit(const Triangle* tri, Ray ray, double t)
    : BaseHit(t), ray(ray), tri(tri)
{ }

HitParams TriangleHit::params() const {
    dvec3 target = ray.O + ray.D * t;

    // interpolate the vertex normals using barycentric coordinates. it
    // corresponds to the ratio of areas of sub-triangles defined by the target
    // point.
    double a3 = length(cross((target - tri->v1), (target - tri->v2))) / 2.0;
    double a1 = length(cross((target - tri->v2), (target - tri->v3))) / 2.0;
    double a2 = length(cross((target - tri->v3), (target - tri->v1))) / 2.0;
    double area = a1 + a2 + a3;
    double c1 = a1 / area;
    double c2 = a2 / area;
    double c3 = a3 / area;
    dvec3 norm = normalize(tri->n1 * c1 + tri->n2 * c2 + tri->n3 * c3);
    dvec2 uv = tri->t1 * c1 + tri->t2 * c2 + tri->t3 * c3;

    // uncomment this for flat shading
    // norm = N;

    return HitParams{ tri, norm, uv };
}

Triangle::Triangle(dvec3 v1, dvec3 v2, dvec3 v3)
    : Triangle(v1, v2, v3, dvec3{}, dvec3{}, dvec3{}, dvec2{}, dvec2{}, dvec2{})
{
    // compute a main normal
    dvec3 d1 = v2 - v1;
    dvec3 d2 = v3 - v1;
    n1 = n2 = n3 = N = normalize(cross(d1, d2));
}

Triangle::Triangle(dvec3 v1, dvec3 v2, dvec3 v3, dvec3 n1, dvec3 n2, dvec3 n3, dvec2 t1, dvec2 t2, dvec2 t3) 
    : v1(v1), v2(v2), v3(v3), n1(n1), n2(n2), n3(n3), t1(t1), t2(t2), t3(t3)
{
    // compute a main normal
    glm::dvec3 d1 = v2 - v1;
    glm::dvec3 d2 = v3 - v1;
    N = glm::normalize(glm::cross(d1, d2));
}

std::unique_ptr<BaseHit> Triangle::intersect(const Ray &ray) const
{
#if 0 // straightforward algorithm

    // test for backface culling: triangle faces away => is invisible
    double proj = dot(ray.D, N);

    // project the ray on the plane surface
    double t = dot((v1 - ray.O), N) / proj;
    dvec3 target = ray.O + ray.D * t;
    
    // test if hit is inside triangle: the point is on the left side of all
    // oriented edges of the triangle.
    // (oriented counter-clockwise from v1 to v2 to v3 to v1)
    if (
        t < 0.0 ||
        dot(N, cross((v2 - v1), (target - v1))) < 0.0 ||
        dot(N, cross((v3 - v2), (target - v2))) < 0.0 ||
        dot(N, cross((v1 - v3), (target - v3))) < 0.0
    ) {
        return Hit::NO_HIT();
    }
    else {
        return std::make_unique<TriangleHit>(this, ray, t);
    }

#else // Moller - Trumbore algorithm
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection.html
    dvec3 v1v2 = v2 - v1;
    dvec3 v1v3 = v3 - v1;
    dvec3 pvec = cross(ray.D, v1v3);
    double det = dot(v1v2, pvec);
    // ray and triangle are parallel if det is close to 0
    // if (fabs(det) < kEpsilon) return false;
    double invDet = 1.0 / det;

    dvec3 tvec = ray.O - v1;
    double u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0) return Hit::NO_HIT();

    dvec3 qvec = cross(tvec, v1v2);
    double v = dot(ray.D, qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) return Hit::NO_HIT();
    
    double t = dot(v1v3, qvec) * invDet;
    if (t < 0.0) return Hit::NO_HIT();
    
    double w = 1.0 - u - v;

    dvec3 norm = normalize(u * n2 + v * n3 + w * n1);
    dvec2 uv = u * t2 + v * t3 + w * t1;
    return std::make_unique<Hit>(t, HitParams{ this, norm, uv });

#endif
}

void Triangle::transform(const dmat4& mat, const dmat4& norm_mat) {
    v1 = mat * dvec4(v1, 1.0);
    v2 = mat * dvec4(v2, 1.0);
    v3 = mat * dvec4(v3, 1.0);
    n1 = norm_mat * dvec4(n1, 1.0);
    n2 = norm_mat * dvec4(n2, 1.0);
    n3 = norm_mat * dvec4(n3, 1.0);

    // compute a main normal
    glm::dvec3 d1 = v2 - v1;
    glm::dvec3 d2 = v3 - v1;
    N = glm::normalize(glm::cross(d1, d2));
}

void Triangle::transform(const dmat4& mat) {
    transform(mat, transpose_inverse(mat));
}

AABB Triangle::computeAABB() const {
    AABB aabb;
    aabb.first = min(v1, v2, v3);
    aabb.second = max(v1, v2, v3);

    return aabb;
}