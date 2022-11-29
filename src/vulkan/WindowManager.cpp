#include "WindowManager.hpp"

#include <stdexcept>
#include "vulkan/vulkan.hpp"


WindowManager::WindowManager(int width, int height) : width(width), height(height), framebufferResized(false) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

WindowManager::~WindowManager() {
    glfwDestroyWindow(window);

    glfwTerminate();

}


void WindowManager::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
}

void WindowManager::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto window_ = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(window));
    window_->framebufferResized = true;
    window_->width = width;
    window_->height = height;
}