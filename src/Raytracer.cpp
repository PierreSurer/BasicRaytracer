#include "Raytracer.hpp"
#include "Material.hpp"
#include <algorithm>
#include <glm.hpp>
#include <glm/gtx/norm.hpp>
#include <fstream>

using namespace glm;

static const double EPS = 0.00001;

Raytracer::Raytracer() : apertureTime(0.0) {
    noise.seed(0);
    srand(0);
}

Color Raytracer::traceColor(const Scene &scene, const Ray &ray, TraceState state)
{
    const TraceParameters &params = scene.params;

    // Find hit object and distance
    auto hit = Hit::NO_HIT();
    for (const auto &o : scene.objects) {
        auto thisHit = o->intersect(ray);
        if (thisHit->t < hit->t) {
            hit = std::move(thisHit);
        }
    }

    // No hit? Return sky color.
    if (hit->no_hit) {
        switch (scene.sky)
        {
            case Sky::DIRECTION:
                return (ray.D + 1.0) * 0.5;
            case Sky::DAY:
                return (ray.D + 1.0) * 0.5;
            case Sky::NIGHT:
            {
                // float y = tan(ray.D.y * glm::pi<float>());
                return clamp(Color((double)noise.simplex2(ray.D * 15.0)) - 0.9, 0.0, 1.0) * 8.0 + clamp(Color((double)noise.simplex2(180.0 + ray.D * 300.0)) - 0.95, 0.0, 1.0) * 15.0;
            }    
            case Sky::NONE:
            default:
                return Color(0.0);
        }
    }


    auto const& hitProp = hit->params();
    auto const& mat = hitProp.obj->material;
    dvec3 hitPoint = ray.at(hit->t);

    Color diffuseColor(0.0), specularColor(0.0);
    Color reflectionColor(0.0), refractionColor(0.0);
    Color finalColor(0.0);

    Color baseColor = mat->texture ?
        mat->texture->sample(hitProp.tex_coords) :
        mat->color;

    for (auto const& light : scene.lights) {

        dvec3 lightVector = light->position - hitPoint; // Not normalized as we need magnitude
        dvec3 lightDir = normalize(lightVector);
        bool inShadow = false;

        if (params.shadows) {
            Ray shadowRay(hitPoint + EPS * lightDir, lightDir, ray.time);
            for (const auto &o : scene.objects) {
                auto shadowHit = o->intersect(shadowRay);
                if(shadowHit->t <= length(lightVector)) {
                    inShadow = true;
                    break;
                }
            }
        }

        if (!inShadow) {
            // Diffuse color calculation
            diffuseColor += baseColor * light->color * clamp(dot(hitProp.normal, lightDir), 0.0, 1.0);

            // Specular color calculation using blinn-phong model
            // blinn
            dvec3 H = normalize(lightDir - ray.D);
            double NdotH = clamp(dot(hitProp.normal, H), 0.0, 1.0);
            specularColor += light->color * pow(NdotH, 4.0 * mat->n);
            // phong
            // dvec3 R = reflect(-lightDir, hit.N);
            // double NdotH = clamp(dot(-ray.D, R), 0.0, 1.0);
            // specularColor = light->color * pow(NdotH, mat->n);

            // inverse-square law (optional)
            // diffuseColor *= light->diffusePower / length2(lightVector);
            // specularColor *= light->specularPower / length2(lightVector);
        }
    }

    // reflection
    if (state.bounces < params.maxBounces && mat->ks * state.reflectionFactor > params.reflectionTheshold) {
        TraceState nextState(state);
        nextState.bounces++;
        nextState.reflectionFactor *= mat->ks;
        dvec3 reflectedDir = reflect(ray.D, hitProp.normal);
        Ray reflectionRay(hitPoint + EPS * reflectedDir, reflectedDir, ray.time);
        reflectionColor = traceColor(scene, reflectionRay, nextState) * mat->ks;
    }
    
    // refraction
    if (state.bounces < params.maxBounces && mat->ior > 1.0) {
        dvec3 refractionDir = refract(ray.D, hitProp.normal, 1.0 / mat->ior);
        if(refractionDir != dvec3(0.0)) { //refraction
            Ray refractionRay = Ray(hitPoint + EPS * refractionDir, refractionDir, ray.time);
            auto refractionHit = hitProp.obj->intersect(refractionRay);
            if(!refractionHit->no_hit) {
                refractionDir = refract(refractionRay.D, -refractionHit->params().normal, mat->ior);
                refractionRay = Ray(refractionRay.at(refractionHit->t) + EPS * refractionDir, refractionDir, ray.time);
            }
            TraceState nextState(state);
            nextState.bounces++;
            if(refractionDir != dvec3(0.0))
                refractionColor = traceColor(scene, refractionRay, nextState);
        }
    }


    // color blending
    finalColor = 
        baseColor * mat->ka +
        diffuseColor * mat->kd +
        specularColor * mat->ks;

    // full reflection
    if(mat->ior <= 1.0) {
        finalColor += reflectionColor;
    }

    // reflection + refraction (fresnel)
    else {
        double kr = mat->ior;
        // fresnel
        double cosi = dot(ray.D, hitProp.normal);
        double etai = 1, etat = mat->ior;
        if (cosi > 0) { std::swap(etai, etat); }
        // Compute sini using Snell's law
        double sint = etai / etat * sqrt(std::max(0.0, 1 - cosi * cosi));
        // Total internal reflection
        if (sint >= 1.0) {
            kr = 1.0;
        } 
        else {
            double cost = sqrt(std::max(0.0, 1 - sint * sint));
            cosi = abs(cosi); 
            double Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
            double Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            kr = (Rs * Rs + Rp * Rp) / 2.0;
        }

        finalColor += reflectionColor * kr + refractionColor * (1.0 - kr);
    }

    return finalColor;
}

Color Raytracer::traceDepth(const Scene &scene, const Ray &ray)
{
    dvec3 axis =  scene.camera.getRotationMat() * glm::dvec4(0.0, 0.0, 1.0, 0.0);
    Ray newRay(ray.at(scene.camera.near), ray.D);

    // Find hit object and distance
    auto min_hit = Hit::NO_HIT();
    for (const auto &obj : scene.objects) {
        auto hit = obj->intersect(ray);
        if (hit->t < min_hit->t) {
            min_hit = std::move(hit);
        }
    }

    // compute distance and project it on the optical axis
    double z = (min_hit->t - scene.camera.near) / (scene.camera.far - scene.camera.near); // Linearized distance
    z = z * dot(axis, ray.D);
    z = clamp(z, 0.0, 1.0);
    Color color = Color(1.0) * (1.0 - z);

    return color;
}

Color Raytracer::traceNormals(const Scene &scene, const Ray &ray)
{
    // Find hit object and distance
    auto min_hit = Hit::NO_HIT();
    for (const auto &obj : scene.objects) {
        auto hit = obj->intersect(ray);
        if (hit->t < min_hit->t) {
            min_hit = std::move(hit);
        }
    }

    // No hit? Return background color.
    if (min_hit->no_hit) return Color(0.0, 0.0, 0.0);

    dvec3 N = min_hit->params().normal;

    return Color(1.0 + N) * 0.5;
}

void Raytracer::render(const Scene& scene, Image& img) {
    switch (scene.params.mode)
    {
    case RenderMode::PHONG:
        return render<RenderMode::PHONG>(scene, img);
    case RenderMode::DEPTH:
        return render<RenderMode::DEPTH>(scene, img);
    case RenderMode::NORMAL:
        return render<RenderMode::NORMAL>(scene, img);
    default:
        throw std::runtime_error("No render type matched");
    } 
}

template <RenderMode Mode>
void Raytracer::render(const Scene& scene, Image& img)
{
    const TraceParameters &params = scene.params;
    int msaa = params.superSampling;
    int64_t w = img.width();
    int64_t h = img.height();

    // build a set of camera axes (right-hand rule, look in z-negative direction)
    glm::dvec3 cam_x =  glm::dvec4(1.0, 0.0, 0.0, 0.0) * scene.camera.getRotationMat();
    glm::dvec3 cam_y =  glm::dvec4(0.0, 1.0, 0.0, 0.0) * scene.camera.getRotationMat();
    glm::dvec3 cam_z =  glm::dvec4(0.0, 0.0, 1.0, 0.0) * scene.camera.getRotationMat(); // -view_direction

    // distance of the focal plane
    double dz = (h - 1) / (2.0 * tan(radians(scene.camera.fov) / 2.0));

    const double offset = 1.0 / (msaa * 2.0);

    #pragma omp parallel for schedule(dynamic, 64)
    for (int64_t i = 0; i < w * h; i++) {
        Color finalColor = {};
        int64_t px = i % w;            // pixel x coordinate
        int64_t py = i / w;            // pixel y coordinate

        double dx = px - w / 2.0;
        double dy = (h - py - 1) - h / 2.0;

        for(int64_t j = 0; j < msaa * msaa; j++) {
            int64_t x = j % msaa;
            int64_t y = j / msaa;
            
            double pxx = dx + (1.0 + 2.0 * x) * offset;
            double pyy = dy + (1.0 + 2.0 * y) * offset;
            glm::dvec3 dir = normalize(-cam_z * dz + cam_x * pxx + cam_y * pyy);
            Color col(0.0);
            Ray ray(scene.camera.getPosition(), dir, apertureTime * (double)rand() / RAND_MAX);

            // this branching is compile-time thanks to the template and constexpr.
            if constexpr (Mode == RenderMode::PHONG)
                col = traceColor(scene, ray);
            else if constexpr (Mode == RenderMode::DEPTH)
                col = traceDepth(scene, ray);
            else if constexpr (Mode == RenderMode::NORMAL)
                col = traceNormals(scene, ray);
            else
                throw std::runtime_error("No render type matched");

            col = clamp(col, 0.0, 1.0);
            finalColor += col;
        }
        finalColor /= msaa * msaa;
        img.putPixel(px, py, 0, (unsigned char)(finalColor.r * 255.0));
        img.putPixel(px, py, 1, (unsigned char)(finalColor.g * 255.0));
        img.putPixel(px, py, 2, (unsigned char)(finalColor.b * 255.0));
        img.putPixel(px, py, 3, 255);
    }
}

