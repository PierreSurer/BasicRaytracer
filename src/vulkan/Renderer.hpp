#pragma once

#include <memory>
#include <vector>

#include "Device.hpp"
#include "SwapChain.hpp"
#include "WindowManager.hpp"
#include "Camera.hpp"

class Renderer
{
public:
    Renderer(WindowManager& window, Device& device);
    ~Renderer();

    Renderer(const Renderer &) = delete;
    Renderer operator=(const Renderer &) = delete;

    VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
    bool isFrameInProgress() const { return isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const {return commandBuffers[currentFrameIndex];}
    int getFrameIndex() const {return currentFrameIndex;}

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    WindowManager& window;
    Device& device;
    std::unique_ptr<SwapChain> swapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    int currentFrameIndex;
    bool isFrameStarted;
    
};
