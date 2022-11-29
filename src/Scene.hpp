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
    Color trace(const Ray &ray);
    void render(Image &img);
};
