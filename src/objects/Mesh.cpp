#include "Mesh.hpp"
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
