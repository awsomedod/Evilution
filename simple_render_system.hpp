#pragma once

#include "evilution_device.hpp"
#include "evilution_pipeline.hpp"
#include "evilution_game_object.hpp"

// std
#include <memory>
namespace evilution {
class SimpleRenderSystem {
  public:
    SimpleRenderSystem(EvilutionDevice& device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<EvilutionGameObject>& gameObjects);

  private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

    EvilutionDevice& evilutionDevice;

    std::unique_ptr<EvilutionPipeline> evilutionPipeline;
    VkPipelineLayout pipelineLayout;
};
} // namespace evilution