#include "Camera.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>


Camera::Camera()
  : fov(22.619864948), //2 atan(200 / 1000)
    near(0.01), far(1000.0),
    up(0.0, 1.0, 0.0), eye(0.0, 0.0, 1000.0), target(0.0, 0.0, 0.0)
{ }

void Camera::lookAt(glm::dvec3 eye, glm::dvec3 target, glm::dvec3 up)
{
    this->eye = eye;
    this->target = target;
    this->up = up;
}
