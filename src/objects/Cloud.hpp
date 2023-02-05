#pragma once

#include "Raycast.hpp"
#include <vector>

static const size_t TEXTURE_SIZE = 32;
static const size_t MIPMAP_LEVEL = 3;

class Cloud {
public:
    Cloud(glm::dvec3 position, glm::dvec3 scale);
    glm::dvec4 traverse(Ray const& ray, double tMaximum);

private:
    double signedDistance(glm::dvec3 localPos);

    uint8_t textures[MIPMAP_LEVEL][TEXTURE_SIZE][TEXTURE_SIZE][TEXTURE_SIZE];

    glm::dvec3 position;
    glm::dvec3 scale;
};

