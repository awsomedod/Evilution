#include "simple_render_system.hpp"
#include "evilution_components.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <entt/entt.hpp>

// std
#include <stdexcept>

namespace evilution {

struct SimplePushConstantData {
    glm::mat2 transform{1.0f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

SimpleRenderSystem::SimpleRenderSystem(EvilutionDevice& device, VkRenderPass renderPass) : evilutionDevice{device} {
    createPipelineLayout();
    createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
    vkDestroyPipelineLayout(evilutionDevice.device(), pipelineLayout, nullptr);
}

void SimpleRenderSystem::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(evilutionDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout != VK_NULL_HANDLE && "Cannot create pipeline before pipeline layout!");

    PipelineConfigInfo pipelineConfig{};
    EvilutionPipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    evilutionPipeline = std::make_unique<EvilutionPipeline>(evilutionDevice, "shaders/simple_shader.vert.spv",
                                                            "shaders/simple_shader.frag.spv", pipelineConfig);
}

void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, entt::registry& registry) {
    evilutionPipeline->bind(commandBuffer);
    auto view = registry.view<Transform2DComponent, RenderComponent>();
    for (entt::entity entity : view) {
        Transform2DComponent& transform2d = view.get<Transform2DComponent>(entity);
        RenderComponent& render = view.get<RenderComponent>(entity);

        transform2d.rotation = glm::mod(transform2d.rotation + 0.01f, glm::two_pi<float>());
        SimplePushConstantData push{};
        push.offset = transform2d.translation;
        push.color = render.color;
        push.transform = transform2d.mat2();
        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(SimplePushConstantData), &push);
        render.model->bind(commandBuffer);
        render.model->draw(commandBuffer);
    }
}
} // namespace evilution