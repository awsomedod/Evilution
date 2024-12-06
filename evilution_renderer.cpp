#include "evilution_renderer.hpp"

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace evilution {

EvilutionRenderer::EvilutionRenderer(EvilutionWindow& window, EvilutionDevice& device)
    : evilutionWindow{window}, evilutionDevice{device} {
    recreateSwapChain();
    createCommandBuffers();
}

EvilutionRenderer::~EvilutionRenderer() { freeCommandBuffers(); }

void EvilutionRenderer::recreateSwapChain() {
    auto extent = evilutionWindow.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = evilutionWindow.getExtent();
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(evilutionDevice.device());

    if (evilutionSwapChain == nullptr) {
        evilutionSwapChain = std::make_unique<EvilutionSwapChain>(evilutionDevice, extent);
    } else {
        std::shared_ptr<EvilutionSwapChain> oldSwapChain = std::move(evilutionSwapChain);
        evilutionSwapChain = std::make_unique<EvilutionSwapChain>(evilutionDevice, extent, oldSwapChain);
        if (!oldSwapChain->compareSwapFormats(*evilutionSwapChain.get())) {
            throw std::runtime_error("Swap chain image(or depth) format has changed!");
        }
    }
    // if reder pass is compatible, do nothing here
    // we'll come back to this in just a moment
}

void EvilutionRenderer::createCommandBuffers() {
    commandBuffers.resize(EvilutionSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = evilutionDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(evilutionDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void EvilutionRenderer::freeCommandBuffers() {
    vkFreeCommandBuffers(evilutionDevice.device(), evilutionDevice.getCommandPool(),
                         static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    commandBuffers.clear();
}

VkCommandBuffer EvilutionRenderer::beginFrame() {
    assert(!isFrameStarted && "Cannot call beginFrame while already in progress");

    auto result = evilutionSwapChain->acquireNextImage(&currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    isFrameStarted = true;

    auto commandBuffer = getCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return commandBuffer;
}

void EvilutionRenderer::endFrame() {
    assert(isFrameStarted && "Cannot call endFrame while frame is not in progress");
    auto commandBuffer = getCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = evilutionSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || evilutionWindow.wasWindowResized()) {
        evilutionWindow.resetWindowResizedFlag();
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % EvilutionSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void EvilutionRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(isFrameStarted && "Cannot call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() &&
           "Cannot begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = evilutionSwapChain->getRenderPass();
    renderPassInfo.framebuffer = evilutionSwapChain->getFrameBuffer(currentImageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = evilutionSwapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(evilutionSwapChain->width());
    viewport.height = static_cast<float>(evilutionSwapChain->height());
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, evilutionSwapChain->getSwapChainExtent()};

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void EvilutionRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(isFrameStarted && "Cannot call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() &&
           "Cannot end render pass on command buffer from a different frame");

    vkCmdEndRenderPass(commandBuffer);
}

} // namespace evilution