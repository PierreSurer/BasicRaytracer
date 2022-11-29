#include "Scene.hpp"
#include "Material.hpp"
#include <algorithm>

Color Scene::traceColor(const Ray &ray)
{
    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(), glm::dvec3());
    Object *obj = nullptr;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t < min_hit.t) {
            min_hit = hit;
            obj = objects[i].get();
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material *material = obj->material.get();            //the hit objects material
    glm::dvec3 hit = ray.at(min_hit.t);                 //the hit point
    glm::dvec3 N = min_hit.N;                          //the normal at hit point
    glm::dvec3 V = -ray.D;                             //the view vector

    double totalIntensity = 0.0f;
    for(auto const& light : lights)
        totalIntensity += light->ambientPower;

    Color color = material->color * material->ka * totalIntensity;
    for(auto const& light : lights) {
        glm::dvec3 lightVector = light->position - hit; // Not normalized as we need magnitude

        // Diffuse color calculation
        Color diffuseColor = material->color * material->kd;
        diffuseColor *= std::clamp(dot(N, normalize(lightVector)), 0.0, 1.0);
        diffuseColor *= light->diffusePower/ (length(lightVector) * length(lightVector));
        color += diffuseColor;

        // Specular color calculation using blinn-phong model
        Color specularColor = Color(1.0) * material->ks;
        glm::dvec3 H = normalize(normalize(lightVector) + V);
        double NdotH = std::clamp(dot(N, H), 0.0, 1.0);
        specularColor *= pow(NdotH, 2.0 * material->n);
        specularColor *= light->specularPower / (length(lightVector) * length(lightVector));
        color += specularColor;
    }

    return color;
}

Color Scene::traceDepth(const Ray &ray, double near, double far)
{
    Ray newRay(ray.at(near), ray.D);

    // Find hit object and distance
    Hit min_hit(far - near, glm::dvec3());
    Object *obj = nullptr;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(newRay));
        if (hit.t < min_hit.t) {
            min_hit = hit;
            obj = objects[i].get();
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);
    double z = 1.0 - min_hit.t / (far - near); // Linearized depth
    Color color = Color(1.0) * z;

    return color;
}

Color Scene::traceNormals(const Ray &ray)
{
    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(), glm::dvec3());
    Object *obj = nullptr;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t < min_hit.t) {
            min_hit = hit;
            obj = objects[i].get();
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    glm::dvec3 N = min_hit.N;                          //the normal at hit point

    return Color(1.0 + N) * 0.5;
}

void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();

    #pragma omp parallel for
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            glm::dvec3 pixel(x + 0.5, h - 1 - y + 0.5, 0);
            Ray ray(eye, normalize(pixel - eye));
            Color col = traceNormals(ray);
            col = clamp(col, 0.0, 1.0);
            img(x, y) = col;
        }
    }
}
