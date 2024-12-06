#include "first_app.hpp"
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std

namespace evilution {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
    SimpleRenderSystem simpleRenderSystem{evilutionDevice, evilutionRenderer.getSwapChainRenderPass()};

    while (!evilutionWindow.shouldClose()) {
        glfwPollEvents();

        if (auto commandBuffer = evilutionRenderer.beginFrame()) {
            evilutionRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
            evilutionRenderer.endSwapChainRenderPass(commandBuffer);
            evilutionRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(evilutionDevice.device());
}

void FirstApp::loadGameObjects() {
    std::vector<EvilutionModel::Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    const std::vector<uint32_t> indices = {0, 1, 2};
    auto evilutionModel = std::make_shared<EvilutionModel>(evilutionDevice, vertices, indices);

    auto traingle = EvilutionGameObject::createGameObject();
    traingle.model = evilutionModel;
    traingle.color = {0.1f, 0.8f, 0.1f};
    traingle.transform2d.translation.x = 0.2f;
    traingle.transform2d.scale = {2.0f, 0.5f};
    traingle.transform2d.rotation = 0.25f * glm::two_pi<float>();
    gameObjects.push_back(std::move(traingle));
}
} // namespace evilution