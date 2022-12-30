#pragma once
#include <glm.hpp>

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

    Material()
    { }
};
