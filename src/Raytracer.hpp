#pragma once

#include "Scene.hpp"
#include "Material.hpp"
#include "Raycast.hpp"
#include "Image.hpp"

class Raytracer {
public:
    void render(const Scene &scene, Image &img);

private:
    Color trace(const Scene &scene, const Ray &ray);
};