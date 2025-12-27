#include "first_app.hpp"
#include "vulkan/vulkan_core.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <array>
#include <chrono>
#include <stdexcept>

namespace evilution {

struct SimplePushConstantData {
        float u_time;
        alignas(8) glm::vec2 u_mouse;  // Normalized mouse position (0-1)
        float u_aspect_ratio; // aspect ratio of the swap chain
};

FirstApp::FirstApp() {
        loadModel();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
}

FirstApp::~FirstApp() { vkDestroyPipelineLayout(evilutionDevice.device(), pipelineLayout, nullptr); }

void FirstApp::run() {
        while (!evilutionWindow.shouldClose()) {
                glfwPollEvents();
                drawFrame();
        }

        vkDeviceWaitIdle(evilutionDevice.device());
}

void FirstApp::sierpinski(std::vector<EvilutionModel::Vertex>& vertices, int depth, glm::vec2 bottomLeft,
                          glm::vec2 bottomRight, glm::vec2 top) {
        if (depth == 0) {
                vertices.push_back(EvilutionModel::Vertex{bottomLeft});
                vertices.push_back(EvilutionModel::Vertex{bottomRight});
                vertices.push_back(EvilutionModel::Vertex{top});
        } else {
                glm::vec2 bottomMid = {(bottomLeft.x + bottomRight.x) / 2.0f, bottomLeft.y};
                glm::vec2 topMidLeft = {(bottomLeft.x + top.x) / 2.0f, (bottomLeft.y + top.y) / 2.0f};
                glm::vec2 topMidRight = {(bottomRight.x + top.x) / 2.0f, (bottomRight.y + top.y) / 2.0f};
                sierpinski(vertices, depth - 1, bottomLeft, bottomMid, topMidLeft);
                sierpinski(vertices, depth - 1, bottomMid, bottomRight, topMidRight);
                sierpinski(vertices, depth - 1, topMidLeft, topMidRight, top);
        }
}

void FirstApp::loadModel() {
        // Full-screen quad covering entire screen in NDC (Normalized Device Coordinates)
        // NDC goes from -1 to 1 in both x and y
        std::vector<EvilutionModel::Vertex> vertices = {
                {{-1.0f, -1.0f}},  // Bottom-left
                {{ 1.0f, -1.0f}},  // Bottom-right
                {{ 1.0f,  1.0f}},  // Top-right
                {{-1.0f,  1.0f}}   // Top-left
        };

        const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
        evilutionModel = std::make_unique<EvilutionModel>(evilutionDevice, vertices, indices);
}

void FirstApp::createPipelineLayout() {
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

        if (vkCreatePipelineLayout(evilutionDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
        }
}

void FirstApp::createPipeline() {
        assert(evilutionSwapChain != nullptr && "Cannot create pipeline before swap chain!");
        assert(pipelineLayout != VK_NULL_HANDLE && "Cannot create pipeline before pipeline layout!");

        PipelineConfigInfo pipelineConfig{};
        EvilutionPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = evilutionSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        evilutionPipeline = std::make_unique<EvilutionPipeline>(evilutionDevice, "shaders/simple_shader.vert.spv",
                                                                "shaders/simple_shader.frag.spv", pipelineConfig);
}

void FirstApp::recreateSwapChain() {
        auto extent = evilutionWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
                extent = evilutionWindow.getExtent();
                glfwWaitEvents();
        }
        vkDeviceWaitIdle(evilutionDevice.device());

        if (evilutionSwapChain == nullptr) {
                evilutionSwapChain = std::make_unique<EvilutionSwapChain>(evilutionDevice, extent);
        } else {
                evilutionSwapChain =
                        std::make_unique<EvilutionSwapChain>(evilutionDevice, extent, std::move(evilutionSwapChain));
                if (evilutionSwapChain->imageCount() != commandBuffers.size()) {
                        freeCommandBuffers();
                        createCommandBuffers();
                }
        }
        // if reder pass is compatible, do nothing here
        createPipeline();
}

void FirstApp::createCommandBuffers() {
        commandBuffers.resize(evilutionSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = evilutionDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(evilutionDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate command buffers!");
        }
}

void FirstApp::freeCommandBuffers() {
        vkFreeCommandBuffers(evilutionDevice.device(), evilutionDevice.getCommandPool(),
                             static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
}

void FirstApp::recordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = evilutionSwapChain->getRenderPass();
        renderPassInfo.framebuffer = evilutionSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = evilutionSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(evilutionSwapChain->width());
        viewport.height = static_cast<float>(evilutionSwapChain->height());
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, evilutionSwapChain->getSwapChainExtent()};

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        evilutionPipeline->bind(commandBuffers[imageIndex]);
        evilutionModel->bind(commandBuffers[imageIndex]);

        // Calculate elapsed time since app start
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(currentTime - startTime).count();

        // Get mouse position
        double mouseX, mouseY;
        glfwGetCursorPos(evilutionWindow.getGLFWwindow(), &mouseX, &mouseY);
        auto extent = evilutionSwapChain->getSwapChainExtent();

        SimplePushConstantData push{};
        push.u_time = time;
        // Normalize mouse to 0-1 range, flip Y so bottom is 0
        push.u_mouse = {
                static_cast<float>(mouseX) / extent.width,
                static_cast<float>(mouseY) / extent.height
        };

        push.u_aspect_ratio = evilutionSwapChain->extentAspectRatio();
        vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(SimplePushConstantData), &push);
        evilutionModel->draw(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);

        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
        }
}

void FirstApp::drawFrame() {
        uint32_t imageIndex;
        auto result = evilutionSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                recreateSwapChain();
                return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                throw std::runtime_error("failed to acquire swap chain image!");
        }

        recordCommandBuffer(imageIndex);
        result = evilutionSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || evilutionWindow.wasWindowResized()) {
                evilutionWindow.resetWindowResizedFlag();
                recreateSwapChain();
                return;
        }

        if (result != VK_SUCCESS) {
                throw std::runtime_error("failed to present swap chain image!");
        }
}

} // namespace evilution