#include "SimpleRenderSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


#include <array>
#include <stdexcept>

struct SimplePushConstantData {
    alignas(4*4*4) glm::mat4 transform;
    alignas(4*4) glm::vec3 offset;
    alignas(4*2) double time;
};

SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass) : device(device) {
    createPipelineLayout();
    createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
  vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

void SimpleRenderSystem::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(device, "shaders/shader_vert.spv", "shaders/shader_frag.spv", pipelineConfig);
}

void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, Camera& camera, std::vector<GameObject>& gameObjects, double worldTime) {
    pipeline->bind(commandBuffer);
    for (auto& obj : gameObjects) {
        SimplePushConstantData push{};
        push.transform = glm::mat4(1.0f);
        push.transform *= camera.getProjection();
        push.transform *= glm::translate(glm::mat4(1.0f), obj.position - glm::vec3(0.0f, camera.position.y, 0.0f));
        push.transform *= glm::scale(glm::mat4(1.0f), obj.size);
        push.transform *= glm::mat4_cast(glm::quat(glm::radians(obj.rotation)));

        push.offset = camera.position;
        push.time = worldTime;

        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(SimplePushConstantData), &push);
        obj.model->bind(commandBuffer);
        obj.model->draw(commandBuffer);
    }
}