#pragma once

#include <glm/glm.hpp>

class Ray
{
public:
    glm::dvec3 O;
    glm::dvec3 D;

    Ray(glm::dvec3 origin, glm::dvec3 direction)
        : O(origin), D(direction)
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
    
    Hit(double t, glm::dvec3 normal, bool no_hit = false)
        : t(t), N(normal), no_hit(no_hit)
    { }

    static const Hit NO_HIT() { 
        constexpr double NaN = std::numeric_limits<double>::infinity();
        static Hit no_hit(NaN, glm::dvec3(0.0), true);
        return no_hit;
    }
};
