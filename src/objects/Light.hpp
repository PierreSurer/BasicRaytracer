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

class Ray
{
public:
    glm::dvec3 O;
    glm::dvec3 D;

    Ray(const glm::dvec3 &from, const glm::dvec3 &dir)
        : O(from), D(dir)
    { }

    glm::dvec3 at(double t) const
    { return O + t * D; }

};

class Hit
{
public:
    double t;
    glm::dvec3 N;
    bool no_hit;
    
    Hit(const double t, const glm::dvec3 &normal, bool nohit = false)
        : t(t), N(normal), no_hit(nohit)
    { }

    static const Hit NO_HIT() { 
        constexpr double NaN = std::numeric_limits<double>::infinity();
        static Hit no_hit(NaN, glm::dvec3(NaN), true);
        return no_hit;
    }

};
