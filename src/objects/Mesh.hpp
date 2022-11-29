#pragma once

#include "Object.hpp"
#include "Triangle.hpp"
#include <vector>

class Mesh : public Object
{
public:
    Mesh(std::vector<Triangle> faces)
        : faces(faces)
    { }

    Hit intersect(const Ray &ray) const;

    std::vector<Triangle> faces;
};
