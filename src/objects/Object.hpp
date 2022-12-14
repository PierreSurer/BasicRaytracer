#pragma once

#include "Light.hpp"

#include <memory>

class Material;

class Object {
public:
    std::shared_ptr<Material> material;

    virtual Hit intersect(const Ray &ray) const = 0;
};
