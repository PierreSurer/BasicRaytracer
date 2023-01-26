#pragma once

#include <iostream>
#include <limits>
#include <glm.hpp>

class Light
{
public:
    Light(glm::dvec3 pos, glm::dvec3 c) : position(pos), color(c)
    { }

    glm::dvec3 position;
    glm::dvec3 color;
    double specularPower = 1000000.0; // Intensity of light on specular component
    double diffusePower = 1000000.0; // Intensity of light on diffuse component
    double ambientPower = 0.3; // Intensity of light on ambient component
};
