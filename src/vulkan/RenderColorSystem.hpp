#pragma once

#include "Device.hpp"
#include "Pipeline.hpp"
#include "Camera.hpp"
#include "objects/Object.hpp"

#include <memory>
#include <vector>

class RenderColorSystem {
public:
    RenderColorSystem(Device &device, VkRenderPass renderPass);
    ~RenderColorSystem();

    RenderColorSystem(const RenderColorSystem &) = delete;
    RenderColorSystem &operator=(const RenderColorSystem &) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, Camera& camera, std::vector<std::unique_ptr<Objects>> const& objects, double worldTime);

private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

    Device& device;

    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
};