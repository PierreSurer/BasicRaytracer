#include "Camera.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>


Camera::Camera() : fov(22.619864948), //2 atan(200 / 1000)
    near(0.01), far(1000.0), 
    position(0.0), rotation(0.0) {
}

Camera::~Camera() {
}

void Camera::setPosition(glm::dvec3 const& pos) {
    this->position = pos;
}

void Camera::setRotation(glm::dvec3 const& rot) {
    this->rotation = rot;
    this->rotMat = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
}

void Camera::lookAt(glm::dvec3 const& eye, glm::dvec3 const& target, glm::dvec3 const& up) {
    glm::dmat4 rotMat = glm::lookAt(eye, target, up);
    glm::quat orientation = glm::quat_cast(rotMat);

    setPosition(eye);
    setRotation(glm::eulerAngles(orientation));
}