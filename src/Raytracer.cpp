#include "Raytracer.hpp"
#include "Material.hpp"
#include <algorithm>
#include <glm.hpp>
#include <glm/gtx/norm.hpp>
#include <fstream>

using namespace glm;

static const double EPS = 0.00001;

Raytracer::Raytracer(std::shared_ptr<Scene> scene) : scene(scene) {

}

Color Raytracer::traceColor(const Ray &ray, TraceState state)
{
    // Find hit object and distance
    Hit hit = Hit::NO_HIT();
    Object *obj = nullptr;
    for (const auto &o : scene->objects) {
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
    dvec3 hitPoint = ray.at(hit.t);        //the hit point
    
    Color color(0.0, 0.0, 0.0);
    color += mat->color * mat->ka;

    for (auto const& light : scene->lights) {

        dvec3 lightVector = light->position - hitPoint; // Not normalized as we need magnitude
        dvec3 lightDir = normalize(lightVector);
        bool inShadow = false;

        if (shadows) {
            Ray shadowRay(hitPoint + EPS * lightDir, lightDir);
            for (const auto &o : scene->objects) {
                Hit shadowHit = o->intersect(shadowRay);
                if(shadowHit.t <= length(lightVector)) {
                    inShadow = true;
                    break;
                }
            }
        }
        // ambient color calculation
        // color += mat->color * mat->ka * light->ambientPower;

        if (!inShadow) {
            // Diffuse color calculation
            Color diffuseColor = light->color * mat->color * mat->kd;
            diffuseColor *= clamp(dot(hit.N, lightDir), 0.0, 1.0);
            // diffuseColor *= light->diffusePower / length2(lightVector);
            color += diffuseColor;

            // Specular color calculation using blinn-phong model
            Color specularColor = light->color * mat->ks;
            // blinn
            dvec3 H = normalize(lightDir - ray.D);
            double NdotH = clamp(dot(hit.N, H), 0.0, 1.0);
            specularColor *= pow(NdotH, 4.0 * mat->n);
            // phong
            // dvec3 R = reflect(-lightDir, hit.N);
            // double NdotH = clamp(dot(-ray.D, R), 0.0, 1.0);
            // specularColor *= pow(NdotH, mat->n);
            // specularColor *= light->specularPower / length2(lightVector);
            color += specularColor;
        }
    }

    Color reflectionColor;
    // reflection
    if (state.bounces < maxBounces && mat->ks * state.reflectionFactor > reflectionTheshold) { // reflection
        state.bounces++;
        state.reflectionFactor *= mat->ks;
        dvec3 reflectedDir = reflect(ray.D, hit.N);
        Ray reflectionRay(hitPoint + EPS * reflectedDir, reflectedDir);
        reflectionColor = traceColor(reflectionRay, state) * mat->ks;
    }
    
    Color refractionColor = color;
    if (state.bounces < maxBounces && mat->refraction > 1.0) {// refraction
        dvec3 refractionDir = refract(ray.D, hit.N, mat->refraction);
        if(refractionDir != dvec3(0.0)) { //refraction
            Ray refractionRay = Ray(hitPoint + EPS * refractionDir, refractionDir);
            Hit refractionHit = obj->intersect(refractionRay);
            if(!refractionHit.no_hit) {
                refractionDir = refract(refractionRay.D, -refractionHit.N, 1.0 / mat->refraction);
                refractionRay = Ray(refractionRay.at(refractionHit.t) + EPS * refractionDir, refractionDir);
            }
            state.bounces++;
            if(refractionDir != dvec3(0.0))
                refractionColor = traceColor(refractionRay, state);
        }

        double kr = mat->refraction;
        // fresnel
        double cosi = dot(ray.D, hit.N); 
        double etai = 1, etat = mat->refraction; 
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

        color += reflectionColor * kr + refractionColor * (1.0 - kr);

    } else {
        color += reflectionColor;
    }

    return color;
}

Color Raytracer::traceDepth(const Ray &ray, const dvec3 &axis, double near, double far)
{
    Ray newRay(ray.at(near), ray.D);

    // Find hit object and distance
    Hit min_hit = Hit::NO_HIT();
    for (const auto &obj : scene->objects) {
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

Color Raytracer::traceNormals(const Ray &ray)
{
    // Find hit object and distance
    Hit min_hit = Hit::NO_HIT();
    for (const auto &obj : scene->objects) {
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

void Raytracer::render(Image &img)
{
    int w = img.width();
    int h = img.height();

    // build a set of camera axes
    dvec3 cam_z = scene->camera.getRotationMat() * glm::dvec4(0.0, 0.0, 1.0, 0.0); // -view_direction
    dvec3 cam_x = scene->camera.getRotationMat() * glm::dvec4(1.0, 0.0, 0.0, 0.0);
    dvec3 cam_y = cross(cam_z, cam_x);

    // distance of the focal plane
    double dz = (h - 1) / (2.0 * tan(radians(scene->camera.getFov()) / 2.0));

    #pragma omp parallel for
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            
            double dx = x - w / 2.0 + 0.5;
            double dy = (h - y - 1) - h / 2.0 + 0.5;
            dvec3 dir = normalize(-cam_z * dz + cam_x * dx + cam_y * dy);
            
            Ray ray(scene->camera.getPosition(), dir);
            // Color col = traceDepth(ray, view_dir, 500, 1000);
            // Color col = traceNormals(ray);
            Color col = traceColor(ray);
            col = clamp(col, 0.0, 1.0);
            img(x, y) = col;
        }
    }
}

bool Raytracer::readParameters(const std::string& inputFilename) {

    // Open file stream for reading and have the YAML module parse it
    std::ifstream fin(inputFilename.c_str());
    if (!fin) {
        std::cerr << "Error: unable to open " << inputFilename << " for reading parameters." << std::endl;;
        return false;
    }
    try {
        YAML::Parser parser(fin);
        if (parser) {
            YAML::Node doc;
            parser.GetNextDocument(doc);

            if (doc.FindValue("Shadows")) doc["Shadows"] >> shadows;
            if (doc.FindValue("MaxRecursionDepth")) doc["MaxRecursionDepth"] >> maxBounces;
            if (doc.FindValue("RecursionThreshold")) doc["RecursionThreshold"] >> reflectionTheshold;

        }
        if (parser) {
            std::cerr << "Warning: unexpected YAML document, ignored." << std::endl;
        }
    } catch(YAML::ParserException& e) {
        std::cerr << "Error at line " << e.mark.line + 1 << ", col " << e.mark.column + 1 << ": " << e.msg << std::endl;
        return false;
    }
    return true;
}
