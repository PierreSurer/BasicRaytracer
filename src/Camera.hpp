#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    void lookAt(glm::dvec3 eye, glm::dvec3 target, glm::dvec3 up = glm::dvec3(0.0, 1.0, 0.0));

    double fov;
    double near;
    double far;
    
    glm::dvec3 up;
    glm::dvec3 eye;
    glm::dvec3 target;
};