#pragma once

#include "Light.hpp"

#include <memory>

class Material;

class Object {
public:
    std::shared_ptr<Material> material;

    void update(const double& dt) {};

    virtual Hit intersect(const Ray &ray) const = 0;
};
