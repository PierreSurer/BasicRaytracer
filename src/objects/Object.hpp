#pragma once

#include <memory>
#include "Light.hpp"
#include <glm/glm.hpp>

class Material;

class Object {
public:
    std::shared_ptr<Material> material;

    virtual Hit intersect(const Ray &ray) const = 0;
};
