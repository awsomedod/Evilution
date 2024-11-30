#pragma once

#include "evilution_device.hpp"
#include "evilution_pipeline.hpp"
#include "evilution_swap_chain.hpp"
#include "evilution_window.hpp"


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

      private:
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();

        EvilutionWindow evilutionWindow{WIDTH, HEIGHT, "Vulkan Tutorial"};
        EvilutionDevice evilutionDevice{evilutionWindow};
        EvilutionSwapChain evilutionSwapChain{evilutionDevice, evilutionWindow.getExtent()};
        std::unique_ptr<EvilutionPipeline> evilutionPipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
};
} // namespace evilution