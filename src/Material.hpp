#pragma once
#include "Image.hpp"
#include <glm.hpp>
#include <memory>

typedef glm::dvec3 Color;
class Material
{
public:
    Color color;        // base color
    double ka;          // ambient intensity
    double kd;          // diffuse intensity
    double ks;          // specular intensity 
    double n;           // exponent for specular highlight size
    double ior = 1.0;   // indice of refraction
    std::shared_ptr<Image> texture = nullptr; // color / albedo texture (optional)

    Material()
    { }
};
