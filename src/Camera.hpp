#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    ~Camera();
    glm::dmat4 getProjection() const { return projection; }


    void setPosition(glm::dvec3 const& pos);
    void setRotation(glm::dvec3 const& rot);
    void lookAt(glm::dvec3 const& eye, glm::dvec3 const& target, glm::dvec3 const& up);

    glm::dvec3 getPosition() const {return position;};
    glm::dvec3 getRotation() const {return rotation;};

    glm::dmat4 getRotationMat() const {return rotMat;};

    double fov;
    double near;
    double far;
    
private:

    glm::dvec3 rotation;
    glm::dvec3 position;

    glm::dmat4 projection;
    glm::dmat4 rotMat;

    
};