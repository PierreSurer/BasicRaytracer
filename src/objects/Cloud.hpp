#pragma once

#include "Raycast.hpp"
#include <vector>

static const size_t textureChannels = 3;

class Cloud {
public:
    Cloud(glm::dvec3 position, glm::dvec3 scale);
    glm::dvec4 traverse(Ray const& ray, double tMaximum);

private:
    double signedDistance(glm::dvec3 localPos);

    std::vector<uint8_t> textures[textureChannels];

    glm::dvec3 position;
    glm::dvec3 scale;

    
};

