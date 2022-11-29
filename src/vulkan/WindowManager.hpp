#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class WindowManager {
public:
    WindowManager(int width, int height);
    ~WindowManager();

    //Can only have one reference to window
    WindowManager(const WindowManager&) = delete;
    WindowManager &operator=(const WindowManager&) = delete;

    bool shouldClose() {return glfwWindowShouldClose(window);};
    VkExtent2D getExtent() {return {(uint32_t)width, (uint32_t)height};}

    bool wasWindowResized() { return framebufferResized; }
    void resetWindowResizedFlag() { framebufferResized = false; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    GLFWwindow* getWindow() {return window;}
private:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    int width;
    int height;
    bool framebufferResized;

    GLFWwindow* window;
    
};
