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

// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<EvilutionModel> createCubeModel(EvilutionDevice& device, glm::vec3 offset) {
    EvilutionModel::Builder modelBuilder{};
    modelBuilder.vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };
    for (auto& v : modelBuilder.vertices) {
        v.position += offset;
    }

    modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                            12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};
    return std::make_unique<EvilutionModel>(device, modelBuilder);
}

void FirstApp::loadGameObjects() {
    std::shared_ptr<EvilutionModel> cubeModel = createCubeModel(evilutionDevice, glm::vec3(0.0f, 0.0f, 0.0f));

    auto cube = evilutionRegistry.create();
    auto& renderComponent = evilutionRegistry.emplace<RenderComponent>(cube);
    auto& transformComponent = evilutionRegistry.emplace<TransformComponent>(cube);

    renderComponent.model = cubeModel;

    transformComponent.translation = {0.0f, 0.0f, 2.5f};
    transformComponent.scale = {0.5f, 0.5f, 0.5f};
}
} // namespace evilution
