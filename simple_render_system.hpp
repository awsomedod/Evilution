#pragma once

#include "evilution_camera.hpp"
#include "evilution_device.hpp"
#include "evilution_pipeline.hpp"

#include <entt/entt.hpp>

// std
#include <memory>
namespace evilution {
class SimpleRenderSystem {
  public:
    SimpleRenderSystem(EvilutionDevice& device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, entt::registry& registry, const EvilutionCamera& camera);

  private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

    EvilutionDevice& evilutionDevice;

    std::unique_ptr<EvilutionPipeline> evilutionPipeline;
    VkPipelineLayout pipelineLayout;
};
} // namespace evilution