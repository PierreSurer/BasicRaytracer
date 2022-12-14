#include "Scene.hpp"
#include "Material.hpp"
#include <algorithm>
#include <glm.hpp>

using namespace glm;

Color Scene::traceColor(const Ray &ray, double reflectionFactor)
{
    // Find hit object and distance
    Hit min_hit = Hit::NO_HIT();
    Object *obj = nullptr;
    for (const auto &o : objects) {
        Hit hit = o->intersect(ray);
        if (hit.t >=0.0 && hit.t < min_hit.t) {
            min_hit = hit;
            obj = o.get();
        }
    }

    // No hit? Return background color.
    if (min_hit.no_hit) return Color(0.0, 0.0, 0.0);

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
        Ray shadowRay(hit + 0.001 * normalize(lightVector), normalize(lightVector));

        bool inShadow = false;
        for (const auto &o : objects) {
            Hit shadowHit = o->intersect(shadowRay);
            if(shadowHit.t > 0 && shadowHit.t <= length(lightVector) + 0.001) {
                inShadow = true;
                break;
            }
            
        }
        if(!inShadow) {
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
        if(material->ks * reflectionFactor > 0.01) { //reflection
            glm::dvec3 reflectedVector = ray.D - 2 * dot(N, ray.D) * N;
            Ray reflectionRay(hit + 0.001 * normalize(reflectedVector), normalize(reflectedVector));
            color = traceColor(reflectionRay, material->ks * reflectionFactor) * material->ks + color * (1.0 - material->ks);

        }
        
    }

    return color;
}

Color Scene::traceDepth(const Ray &ray, const glm::dvec3 &axis, double near, double far)
{
    Ray newRay(ray.at(near), ray.D);

    // Find hit object and distance
    Hit min_hit = Hit::NO_HIT();
    for (const auto &obj : objects) {
        Hit hit = obj->intersect(ray);
        if (hit.t < min_hit.t) {
            min_hit = hit;
        }
    }

    // compute distance and project it on the optical axis
    double z = (min_hit.t - near) / (far - near); // Linearized distance
    z = z * glm::dot(axis, ray.D);
    z = glm::clamp(z, 0.0, 1.0);
    Color color = Color(1.0) * (1.0 - z);

    return color;
}

Color Scene::traceNormals(const Ray &ray)
{
    // Find hit object and distance
    Hit min_hit = Hit::NO_HIT();
    for (const auto &obj : objects) {
        Hit hit = obj->intersect(ray);
        if (hit.t < min_hit.t) {
            min_hit = hit;
        }
    }

    // No hit? Return background color.
    if (min_hit.no_hit) return Color(0.0, 0.0, 0.0);

    glm::dvec3 N = min_hit.N;                          //the normal at hit point

    return Color(1.0 + N) * 0.5;
}

void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();

    // build a set of camera axis
    dvec3 up = dvec3(0.0, 1.0, 0.0);
    dvec3 cam_z = -normalize(dvec3(0.0) - eye); // looking at the origin
    dvec3 cam_x = cross(up, cam_z);
    dvec3 cam_y = cross(cam_z, cam_x);

    double dz = h / (2.0 * sin(fov / 2.0));

    #pragma omp parallel for
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            
            double dx = x - w / 2.0;
            double dy = y - h / 2.0;
            dvec3 dir = normalize(cam_z * dz + cam_x * dx + cam_y * dy);
            
            Ray ray(eye, dir);
            // Color col = traceDepth(ray, view_dir, 500, 1000);
            // Color col = traceNormals(ray);
            Color col = traceColor(ray, 1.0);
            col = clamp(col, 0.0, 1.0);
            img(x, y) = col;
        }
    }
}
