#pragma once

#include <vector>
#include "objects/Light.hpp"
#include "objects/Object.hpp"
#include "Image.hpp"

class Scene
{
public:
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Light>> lights;
    glm::dvec3 eye;
    Color traceColor(const Ray &ray, double reflectionFactor);
    Color traceDepth(const Ray &ray, const glm::dvec3 &axis, double near, double far);
    Color traceNormals(const Ray &ray);
    void render(Image &img);
};
