#pragma once

#include <iostream>
#include <string>
#include "objects/Light.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "yaml/yaml.h"

enum class RenderMode {PHONG, NORMAL, DEPTH};

struct TraceState
{
    double reflectionFactor = 1.0;
    int bounces = 0;
};

class Raytracer {
public:
    Raytracer(std::shared_ptr<Scene> scene);

    void render(Image &img);
    bool readParameters(const std::string& inputFilename);
private:
    Color traceColor(const Ray &ray, TraceState state = TraceState{});
    Color traceDepth(const Ray &ray, const glm::dvec3 &axis, double near, double far);
    Color traceNormals(const Ray &ray);

    std::shared_ptr<Scene> scene; //TODO remove
    bool shadows = true;
    double reflectionTheshold = 0.01;
    int maxBounces = 10;
    int superSampling = 1;
    RenderMode mode = RenderMode::PHONG;

};