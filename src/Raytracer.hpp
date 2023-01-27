#pragma once

#include <iostream>
#include <string>
#include "objects/Light.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "yaml/yaml.h"
#include "noise/Simplex.hpp"

struct TraceState {
    double reflectionFactor = 1.0;
    int bounces = 0;
};

class Raytracer {
public:
    Raytracer();
    void render(const Scene &scene, Image &img);

private:
    Color traceColor(const Scene &scene, const Ray &ray, TraceState state = TraceState{});
    Color traceDepth(const Scene &scene, const Ray &ray);
    Color traceNormals(const Scene &scene, const Ray &ray);

    SimplexNoise noise;

};