#pragma once

#include "evilution_device.hpp"
#include "evilution_swap_chain.hpp"
#include "evilution_window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>


namespace evilution {
class EvilutionRenderer {
  public:
    EvilutionRenderer(EvilutionWindow& window, EvilutionDevice& device);
    ~EvilutionRenderer();

    EvilutionRenderer(const EvilutionRenderer&) = delete;
    EvilutionRenderer& operator=(const EvilutionRenderer&) = delete;

    VkRenderPass getSwapChainRenderPass() const { return evilutionSwapChain->getRenderPass(); }
    bool isFrameInProgress() const { return isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const {
        assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
        return commandBuffers[currentFrameIndex];
    }

    int getFrameIndex() const {
        assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
        return currentFrameIndex;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    EvilutionWindow& evilutionWindow;
    EvilutionDevice& evilutionDevice;
    std::unique_ptr<EvilutionSwapChain> evilutionSwapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    int currentFrameIndex{0};
    bool isFrameStarted{false};
};
} // namespace evilution