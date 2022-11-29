#pragma once

#include "Device.hpp"
#include "GameObject.hpp"
#include "Pipeline.hpp"
#include "Camera.hpp"

#include <memory>
#include <vector>

class SimpleRenderSystem {
public:
    SimpleRenderSystem(Device &device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, Camera& camera, std::vector<GameObject> &gameObjects, double worldTime);

private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

    Device& device;

    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
};