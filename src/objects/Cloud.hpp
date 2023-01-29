#pragma once

#include "Raycast.hpp"
#include <vector>

typedef glm::dvec3 Color;

class Cloud {
public:
    Cloud(glm::dvec3 position, glm::dvec3 scale, int particlesNum);
    Color traverse(Ray const& ray);

private:
    struct Particle {
        glm::dvec3 pos;
        double size;
    };
    double calculateOpacity(Ray const& ray, Particle const& p);

    glm::dvec3 position;
    glm::dvec3 scale;

    std::vector<Particle> particles;
};

