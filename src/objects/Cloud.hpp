#pragma once

#include "Raycast.hpp"
#include <vector>

class Cloud {
public:
    Cloud(glm::dvec3 position, glm::dvec3 scale);
    glm::dvec4 traverse(Ray const& ray);

private:
    double signedDistance(glm::dvec3 localPos);
    double densityFunction(double sdf);

    glm::dvec3 position;
    glm::dvec3 scale;
};

