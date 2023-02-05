#pragma once
#include "Image.hpp"
#include <glm.hpp>
#include <memory>

typedef glm::dvec3 Color;
class Material
{
public:
    Color color = Color(1.0);                 // base color (ambient and diffuse)
    double ka = 0.0;                          // ambient intensity
    double kd = 0.0;                          // diffuse intensity
    double ks = 0.0;                          // specular intensity 
    double n = 0.0;                           // exponent for specular highlight size
    double ior = 1.0;                         // index of refraction
    std::shared_ptr<Image> diffuseMap = nullptr; // color / albedo texture (optional)
    std::shared_ptr<Image> normalMap = nullptr; // normal / bump texture (optional)
    std::shared_ptr<Image> specularMap = nullptr; // normal / bump texture (optional)

    Material()
    { }
};

static std::shared_ptr<Material> DEFAULT_MATERIAL = std::make_shared<Material>();
