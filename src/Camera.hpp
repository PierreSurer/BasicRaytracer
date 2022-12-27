#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//#include "WindowManager.hpp"

class Camera
{
public:
    Camera();
    ~Camera();

    //void update(WindowManager* window, float dt);
    glm::dmat4 getProjection() const{ return projection; }
    double getFov() const { return fov; }

    void setPosition(glm::dvec3 const& pos);
    void setRotation(glm::dvec3 const& rot);
    void lookAt(glm::dvec3 const& eye, glm::dvec3 const& target, glm::dvec3 const& up);

    glm::dvec3 getPosition() const {return position;};
    glm::dvec3 getRotation() const {return rotation;};

    glm::dmat4 getRotationMat() const {return rotMat;};

    // struct KeyMappings {
    //     int moveLeft = GLFW_KEY_A;
    //     int moveRight = GLFW_KEY_D;
    //     int moveForward = GLFW_KEY_W;
    //     int moveBackward = GLFW_KEY_S;
    //     int moveUp = GLFW_KEY_E;
    //     int moveDown = GLFW_KEY_Q;
    //     int lookLeft = GLFW_KEY_LEFT;
    //     int lookRight = GLFW_KEY_RIGHT;
    //     int lookUp = GLFW_KEY_UP;
    //     int lookDown = GLFW_KEY_DOWN;
    // };
    
private:

    glm::dvec3 rotation;
    glm::dvec3 position;
    // KeyMappings keys;
    double moveSpeed;
    double lookSpeed;

    glm::dmat4 projection;
    glm::dmat4 rotMat;
    double fov;
    double near;
    double far;
    
};

