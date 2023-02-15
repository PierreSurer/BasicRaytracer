#include "Raytracer.hpp"
#include "Material.hpp"

#include <glm/glm.hpp>

using namespace glm;

Color Raytracer::trace(const Scene &scene, const Ray &ray)
{
    // Find hit object and distance
    Hit min_hit = Hit::NO_HIT();
    Object *obj = nullptr;
    for (const auto& object: scene.objects) {
        Hit hit(object->intersect(ray));
        if (hit.t<min_hit.t) {
            min_hit = hit;
            obj = object.get();
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    auto& material = obj->material;                //the hit objects material
    auto hit = ray.at(min_hit.t);                  //the hit point
    auto N = min_hit.N;                            //the normal at hit point
    auto V = -ray.D;                               //the view vector


    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints:
    *        dot(v1, v2)           dot product
    *        v1 + v2               vector sum
    *        v1 - v2               vector difference
    *        normalize(v)          normalizes vector, returns length
    *        double * Color        scales each color component (r,g,b)
    *        Color * Color         dito
    *        pow(a,b)              a to the power of b
    ****************************************************/

    Color color = material->color;                  // place holder

    return color;
}

void Raytracer::render(const Scene& scene, Image& img)
{
    dvec3 eye = scene.parameters.eye;
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            dvec3 pixel(x+0.5, h-1-y+0.5, 0.0);
            Ray ray(eye, normalize(pixel-eye));
            Color col = trace(scene, ray);
            img(x, y) = clamp(col, 0.0, 1.0);
        }
    }
}

