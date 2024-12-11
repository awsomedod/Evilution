#include "first_app.hpp"

#include "evilution_model.hpp"
#include "simple_render_system.hpp"
#include "evilution_components.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <cassert>

namespace evilution {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}
 
void FirstApp::run() {
  SimpleRenderSystem simpleRenderSystem{evilutionDevice, evilutionRenderer.getSwapChainRenderPass()};

  while (!evilutionWindow.shouldClose()) {
    glfwPollEvents();

    if (auto commandBuffer = evilutionRenderer.beginFrame()) {
      evilutionRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, evilutionRegistry);
      evilutionRenderer.endSwapChainRenderPass(commandBuffer);
      evilutionRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(evilutionDevice.device());
}

void FirstApp::loadGameObjects() {
  std::vector<EvilutionModel::Vertex> vertices{
      {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
  std::vector<uint32_t> indices = {0, 1, 2};
  auto triangleModel = std::make_shared<EvilutionModel>(evilutionDevice, vertices, indices);

  auto triangle = evilutionRegistry.create();
  auto& renderComponent = evilutionRegistry.emplace<RenderComponent>(triangle);
  auto& transform2DComponent = evilutionRegistry.emplace<Transform2DComponent>(triangle);
  
  renderComponent.model = triangleModel;
  renderComponent.color = {.1f, .8f, .1f};
  
  transform2DComponent.translation.x = 0.2f;
  transform2DComponent.scale = {2.f, .5f};
  transform2DComponent.rotation = .25f * glm::two_pi<float>();
}
}  // namespace lve
