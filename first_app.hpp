#pragma once

#include "evilution_device.hpp"
#include "evilution_pipeline.hpp"
#include "evilution_swap_chain.hpp"
#include "evilution_window.hpp"
#include "evilution_model.hpp"


// std
#include <memory>
#include <vector>
namespace evilution {
class FirstApp {
      public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp&) = delete;
        FirstApp& operator=(const FirstApp&) = delete;

        void run();

        void sierpinski(
            std::vector<EvilutionModel::Vertex> &vertices,
            int depth,
            glm::vec2 left,
            glm::vec2 right,
            glm::vec2 top);

      private:
        void loadModel();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);

        EvilutionWindow evilutionWindow{WIDTH, HEIGHT, "Vulkan Tutorial"};
        EvilutionDevice evilutionDevice{evilutionWindow};
        std::unique_ptr<EvilutionSwapChain> evilutionSwapChain;
        std::unique_ptr<EvilutionPipeline> evilutionPipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<EvilutionModel> evilutionModel;
};
} // namespace evilution