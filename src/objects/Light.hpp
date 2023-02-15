#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

class Light
{
public:
    Light(glm::dvec3 pos, Color c) : position(pos), color(c)
    { }

    glm::dvec3 position;
    Color color;
};
