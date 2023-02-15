#pragma once

#include "Material.hpp"
#include "Raycast.hpp"

#include <memory>

class Object {
public:
    std::shared_ptr<Material> material = DEFAULT_MATERIAL;
    virtual ~Object() = default;

    virtual Hit intersect(const Ray &ray) const = 0;
};
