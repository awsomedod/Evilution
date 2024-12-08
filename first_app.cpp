#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "evilution_components.hpp"
#include "gravity_physics_system.hpp"
#include "vec2_field_system.hpp"

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
    GravityPhysicsSystem gravitySystem{0.81f};

    while (!evilutionWindow.shouldClose()) {
        glfwPollEvents();

        if (auto commandBuffer = evilutionRenderer.beginFrame()) {
            //update systems
            gravitySystem.update(evilutionRegistry, 1.f / 60, 50);

            //render system
            evilutionRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, evilutionRegistry);
            evilutionRenderer.endSwapChainRenderPass(commandBuffer);
            evilutionRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(evilutionDevice.device());
}

void FirstApp::loadGameObjects() {
    // std::vector<EvilutionModel::Vertex> vertices = {
    //     {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    //     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    //     {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    // };
    // const std::vector<uint32_t> indices = {0, 1, 2};
    // auto evilutionModel = std::make_shared<EvilutionModel>(evilutionDevice, vertices, indices);

    // entt::entity traingle = evilutionRegistry.create();
    
    // Transform2DComponent& transform = evilutionRegistry.emplace<Transform2DComponent>(traingle);
    // transform.translation.x = 0.2f;
    // transform.scale = {2.0f, 0.5f};
    // transform.rotation = 0.25f * glm::two_pi<float>();

    // RenderComponent& render = evilutionRegistry.emplace<RenderComponent>(traingle, evilutionModel);
    // render.model = evilutionModel;
    // render.color = {0.1f, 0.8f, 0.1f};
    std::shared_ptr<EvilutionModel> circleModel = Vec2FieldSystem::createCircleModel(evilutionDevice, 64);

    auto redCircle = evilutionRegistry.create();
    Transform2DComponent& redTransform = evilutionRegistry.emplace<Transform2DComponent>(redCircle);
    redTransform.translation = {0.f, 0.f};
    redTransform.scale = glm::vec2{.05f};
    RigidBody2dComponent& redRigidbody = evilutionRegistry.emplace<RigidBody2dComponent>(redCircle);
    redRigidbody.velocity = glm::vec2{0.f, 0.f};
    redRigidbody.mass = 1.f;
    RenderComponent& redRender = evilutionRegistry.emplace<RenderComponent>(redCircle);
    redRender.model = circleModel;
    redRender.color = glm::vec3{1.0f, 0.0f, 0.0f};

    auto blueCircle = evilutionRegistry.create();
    Transform2DComponent& blueTransform = evilutionRegistry.emplace<Transform2DComponent>(blueCircle);
    blueTransform.translation = {-.45f, 0.f};
    blueTransform.scale = glm::vec2{.05f};
    RigidBody2dComponent& blueRigidbody = evilutionRegistry.emplace<RigidBody2dComponent>(blueCircle);
    blueRigidbody.velocity = glm::vec2{0.f, 0.f};
    blueRigidbody.mass = 1.f;
    RenderComponent& blueRender = evilutionRegistry.emplace<RenderComponent>(blueCircle);
    blueRender.model = circleModel;
    blueRender.color = glm::vec3{0.0f, 0.0f, 1.0f};
}
} // namespace evilution