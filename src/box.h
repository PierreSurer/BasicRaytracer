#pragma once

#include "object.h"

class Box : public Object
{
public:
    Box(Point position, double size) : position(position), size(size) { }

    virtual Hit intersect(const Ray &ray);

    const Point position;
    const double size;

};
