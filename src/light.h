//
//  Framework for a raytracer
//  File: light.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Author: Maarten Everts
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#pragma once

#include <iostream>
#include <limits>
#include "triple.h"

class Light
{
public:
    Light(Point pos, Color c) : position(pos), color(c)
    { }

    Point position;
    Color color;
    float specularPower = 2000000.0f; // Intensity of light on specular component
    float diffusePower = 2000000.0f; // Intensity of light on diffuse component
    float ambientPower = 0.5f; // Intensity of light on ambient component
};

class Ray
{
public:
    Point O;
    Vector D;

    Ray(Point from, Vector dir)
        : O(from), D(dir)
    { }

    Point at(double t) const
    { return O + t * D; }

};

class Hit
{
public:
    double t;
    Vector N;
    bool no_hit;
    
    Hit(const double t, const Vector &normal, bool nohit = false)
        : t(t), N(normal), no_hit(nohit)
    { }

    static const Hit NO_HIT()
    {
        constexpr double NaN = std::numeric_limits<double>::quiet_NaN();
        static Hit no_hit(NaN, Vector(NaN, NaN, NaN), true);
        return no_hit;
    }
};
