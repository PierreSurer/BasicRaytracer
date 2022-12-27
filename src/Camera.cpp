#include "Camera.hpp"

#include <limits>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>


Camera::Camera() : fov(45.0), near(0.01), far(500.0), 
    position(0.0), rotation(0.0),
    moveSpeed(3.0), lookSpeed(120.0) {
}

Camera::~Camera() {
}

void Camera::setPosition(glm::dvec3 const& pos) {
    this->position = pos;
}

void Camera::setRotation(glm::dvec3 const& rot) {
    this->rotation = rot;
    this->rotation.y = glm::mod(rotation.y, 360.0);
    this->rotMat = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
}

void Camera::lookAt(glm::dvec3 const& eye, glm::dvec3 const& target, glm::dvec3 const& up) {
    glm::dmat4 rotMat = glm::lookAt(eye, target, up);
    glm::quat orientation = glm::quat_cast(rotMat);

    setPosition(eye);
    setRotation(glm::eulerAngles(orientation));
}

// void Camera::update(WindowManager* manager, float dt) {
//     auto window = manager->getWindow();
//     if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotation.y += lookSpeed * dt;
//     if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)  rotation.y -= lookSpeed * dt;
//     if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)  rotation.x += lookSpeed * dt;
//     if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)  rotation.x -= lookSpeed * dt;

//     rotation.y = glm::mod(rotation.y, 360.0f);
//     auto extent = manager->getExtent();
//     if(extent.width != 0 && extent.height != 0)
//         projection = glm::perspective(fov, (float)std::max(extent.width, extent.height) / (float)std::min(extent.width, extent.height), near, far);//TODO

//     glm::mat4 transform{1.0f};
//     transform = glm::rotate(transform, glm::radians(rotation.x), {1.0f, 0.0f, 0.0f});
//     transform = glm::rotate(transform, glm::radians(rotation.y), {0.0f, 1.0f, 0.0f});

//     projection *= transform;

//     glm::vec3 moveDir{0.f};
//     if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir -= glm::vec3(0.0f, 0.0f, 1.0f);
//     if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir += glm::vec3(0.0f, 0.0f, 1.0f);
//     if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += glm::vec3(1.0f, 0.0f, 0.0f);
//     if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= glm::vec3(1.0f, 0.0f, 0.0f);
//     if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
//     if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);

//     moveDir = glm::vec4(moveDir, 1.0f) * transform;

//     if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
//         position += moveSpeed * dt * glm::normalize(moveDir);
//     }

// }