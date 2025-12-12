#include "first_app.hpp"

#include "evilution_camera.hpp"
#include "evilution_components.hpp"
#include "evilution_model.hpp"
#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"


// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <cassert>
#include <chrono>

namespace evilution {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
    SimpleRenderSystem simpleRenderSystem{evilutionDevice, evilutionRenderer.getSwapChainRenderPass()};
    EvilutionCamera camera{};

    auto cameraTransform = TransformComponent{};
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!evilutionWindow.shouldClose()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        cameraController.moveInPlaneXZ(evilutionWindow.getGLFWwindow(), frameTime, cameraTransform);
        camera.setViewYXZ(cameraTransform.translation, cameraTransform.rotation);

        float aspect = evilutionRenderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

        if (auto commandBuffer = evilutionRenderer.beginFrame()) {
            evilutionRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, evilutionRegistry, camera);
            evilutionRenderer.endSwapChainRenderPass(commandBuffer);
            evilutionRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(evilutionDevice.device());
}

void FirstApp::loadGameObjects() {
    std::shared_ptr<EvilutionModel> vaseModel = EvilutionModel::createModelFromFile(evilutionDevice, "models/flat_vase.obj");

    auto gameObject = evilutionRegistry.create();
    auto& renderComponent = evilutionRegistry.emplace<RenderComponent>(gameObject);
    auto& transformComponent = evilutionRegistry.emplace<TransformComponent>(gameObject);

    renderComponent.model = vaseModel;

    transformComponent.translation = {0.0f, 0.0f, 2.5f};
    transformComponent.scale = {3.f, 1.5f, 3.f};
}
} // namespace evilution
