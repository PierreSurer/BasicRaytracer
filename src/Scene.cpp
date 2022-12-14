#include "Scene.hpp"
#include "Material.hpp"
#include <algorithm>
#include <glm.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;

static const double EPS = 0.001;


Color Scene::traceColor(const Ray &ray, double reflectionFactor)
{
    // Find hit object and distance
    Hit hit = Hit::NO_HIT();
    Object *obj = nullptr;
    for (const auto &o : objects) {
        Hit thisHit = o->intersect(ray);
        if (thisHit.t < hit.t) {
            hit = thisHit;
            obj = o.get();
        }
    }

    // No hit? Return background color.
    if (hit.no_hit)
        return Color(0.0, 0.0, 0.0);

    auto const& mat = obj->material;       //the hit objects material
    dvec3 hitPoint = ray.at(hit.t);            //the hit point
    
    double totalIntensity = 0.0;
    for (auto const& light : lights)
        totalIntensity += light->ambientPower;

    Color color(0.0, 0.0, 0.0);

    // ambient color, only if direct ray
    color += mat->color * mat->ka * totalIntensity;

    for (auto const& light : lights) {
        dvec3 lightVector = light->position - hitPoint; // Not normalized as we need magnitude
        dvec3 lightDir = normalize(lightVector);
        Ray shadowRay(hitPoint + EPS * lightDir, lightDir);

        bool inShadow = false;
        for (const auto &o : objects) {
            Hit shadowHit = o->intersect(shadowRay);
            if(shadowHit.t <= length(lightVector)) {
                inShadow = true;
                break;
            }
        }

        if (!inShadow) {
            // Diffuse color calculation
            Color diffuseColor = mat->color * mat->kd;
            diffuseColor *= clamp(dot(hit.N, lightDir), 0.0, 1.0);
            diffuseColor *= light->diffusePower / length2(lightVector);
            color += diffuseColor;

            // Specular color calculation using blinn-phong model
            Color specularColor = Color(1.0) * mat->ks;
            dvec3 H = normalize(lightDir - ray.D);
            double NdotH = clamp(dot(hit.N, H), 0.0, 1.0);
            specularColor *= pow(NdotH, 2.0 * mat->n);
            specularColor *= light->specularPower / length2(lightVector);
            color += specularColor;
        }

        // reflection
        if (mat->ks * reflectionFactor > 0.01) { // reflection
            dvec3 reflectedDir = reflect(ray.D, hit.N);
            Ray reflectionRay(hitPoint + EPS * reflectedDir, reflectedDir);
            color += traceColor(reflectionRay, mat->ks * reflectionFactor) * mat->ks;
        }
    }

    return color;
}

Color Scene::traceDepth(const Ray &ray, const dvec3 &axis, double near, double far)
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
    z = z * dot(axis, ray.D);
    z = clamp(z, 0.0, 1.0);
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

    dvec3 N = min_hit.N;                          //the normal at hit point

    return Color(1.0 + N) * 0.5;
}

void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();

    // build a set of camera axes
    dvec3 cam_z = normalize(eye - target); // -view_direction
    dvec3 cam_x = cross(up, cam_z);
    dvec3 cam_y = cross(cam_z, cam_x);

    // distance of the focal plane
    double dz = (h - 1) / (2.0 * tan(radians(fov) / 2.0));

    #pragma omp parallel for
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            
            double dx = x - w / 2.0 + 0.5;
            double dy = (h - y - 1) - h / 2.0 + 0.5;
            dvec3 dir = normalize(-cam_z * dz + cam_x * dx + cam_y * dy);
            
            Ray ray(eye, dir);
            // Color col = traceDepth(ray, view_dir, 500, 1000);
            // Color col = traceNormals(ray);
            Color col = traceColor(ray);
            col = clamp(col, 0.0, 1.0);
            img(x, y) = col;
        }
    }
}
