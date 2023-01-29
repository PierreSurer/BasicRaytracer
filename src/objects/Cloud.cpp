#include "Cloud.hpp"

#include <random>
#include <glm/gtx/norm.hpp>

Cloud::Cloud(glm::dvec3 position, glm::dvec3 scale, int particlesNum)
 : position(position), scale(scale) {
    // Initialiser la génération aléatoire
    std::default_random_engine generator;
    std::normal_distribution<double> posDistribution(0.0, 0.2);
    std::uniform_real_distribution<double> sizeDistribution(0.0, 2.0);

    for (int i = 0; i < particlesNum; i++) {
        Particle p;
        p.pos = glm::dvec3(posDistribution(generator) * scale.x * 0.5, posDistribution(generator) * scale.y * 0.5, posDistribution(generator) * scale.z * 0.5);
        p.size = sizeDistribution(generator);
        particles.push_back(p);
    }
}


Color Cloud::traverse(Ray const& ray) {
    Color particleColor = Color(1.0); // couleur de la particule pour ce pixel

    auto first = position - scale * 0.5;
    auto second = position + scale * 0.5;

    // test regular intersection
    glm::dvec3 tMin = (first - ray.O) / ray.D;
    glm::dvec3 tMax = (second - ray.O) / ray.D;
    glm::dvec3 t1 = min(tMin, tMax);
    glm::dvec3 t2 = max(tMin, tMax);
    double tNear = compMax(t1);
    double tFar = compMin(t2);
    if(tNear > tFar || tFar < 0.0) return particleColor;


    for (Particle p : particles) {
        particleColor *= 1.0 + 0.005 * calculateOpacity(ray, p);
    }

    if(particleColor != Color(1.0)) {
        int smplDist = 10;
        int smplCount = 1 + (int)(tFar - tNear) / smplDist;
        for(int i = 0; i < smplCount; i++) {
            auto pos = ray.at(tNear + smplDist * i);
            Ray shadowray(pos, glm::dvec3(0.0, 1.0, 0.0), ray.time);
            for (Particle p : particles)
                particleColor *= 1.0 - 0.001 * calculateOpacity(shadowray, p);
            particleColor *= 1.005;
        }
    }
    

    return particleColor;
}

double Cloud::calculateOpacity(Ray const& ray, Particle const& p) {
    glm::dvec3 OC = p.pos + position - ray.O;
    double t = dot(OC, ray.D);
    if(t < 0.0) return 0.0;
    
    double h_2 = glm::length2(OC) - t * t;
    double r2 = p.size * p.size;
    if (h_2 > r2) {
        return 0.0;
    }

    return 2.0 * sqrt(r2 - h_2);
}