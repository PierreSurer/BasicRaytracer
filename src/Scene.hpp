#pragma once

#include "objects/Light.hpp"
#include "objects/Object.hpp"
#include "Image.hpp"

#include <vector>
#include <glm.hpp>

enum class RenderMode {PHONG, NORMAL, DEPTH};
struct TraceOptions
{
    bool shadows = true;
    double reflectionTheshold = 0.01;
    int maxBounces = 10;
    int superSampling = 1;
    RenderMode mode = RenderMode::PHONG;
};

struct TraceState
{
    double reflectionFactor = 1.0;
    int bounces = 0;
};

class Scene
{
public:
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Light>> lights;
    glm::dvec3 eye, target, up;
    double fov = 22.619864948; //2 atan(200 / 1000)
    TraceOptions options;

    Color traceColor(const Ray &ray, TraceState state = TraceState{});
    Color traceDepth(const Ray &ray, const glm::dvec3 &axis, double near, double far);
    Color traceNormals(const Ray &ray);
    void render(Image &img);
};
