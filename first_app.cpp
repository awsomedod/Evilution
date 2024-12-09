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
    Vec2FieldSystem vecFieldSystem{};

    while (!evilutionWindow.shouldClose()) {
        glfwPollEvents();

        if (auto commandBuffer = evilutionRenderer.beginFrame()) {
            //update systems
            gravitySystem.update(evilutionRegistry, 1.f / 60, 5);
            vecFieldSystem.update(gravitySystem, evilutionRegistry, vec2FieldRegistry);

            //render system
            evilutionRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, evilutionRegistry);
            simpleRenderSystem.renderGameObjects(commandBuffer, vec2FieldRegistry);
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
    std::shared_ptr<EvilutionModel> squareModel = Vec2FieldSystem::createSquareModel(evilutionDevice, glm::vec2{.5f, .0f});

    auto redCircle = evilutionRegistry.create();
    Transform2DComponent& redTransform = evilutionRegistry.emplace<Transform2DComponent>(redCircle);
    RigidBody2dComponent& redRigidbody = evilutionRegistry.emplace<RigidBody2dComponent>(redCircle);
    RenderComponent& redRender = evilutionRegistry.emplace<RenderComponent>(redCircle);
    
    redTransform.scale = glm::vec2{.05f};
    redTransform.translation = {-.7f, .0f};
    redRender.color = glm::vec3{1.0f, 0.0f, 0.0f};
    redRigidbody.velocity = glm::vec2{0.0f, -.5f};
    redRigidbody.mass = 0.25f;
    redRender.model = circleModel;
  
    auto blueCircle = evilutionRegistry.create();
    Transform2DComponent& blueTransform = evilutionRegistry.emplace<Transform2DComponent>(blueCircle);
    RigidBody2dComponent& blueRigidbody = evilutionRegistry.emplace<RigidBody2dComponent>(blueCircle);
    RenderComponent& blueRender = evilutionRegistry.emplace<RenderComponent>(blueCircle);

    blueTransform.scale = glm::vec2{.05f};
    blueTransform.translation = {0.0f, .0f};
    blueRender.color = glm::vec3{0.0f, 0.0f, 1.0f};
    blueRigidbody.velocity = glm::vec2{0.0f, .0f};
    blueRigidbody.mass = 0.25f;
    blueRender.model = circleModel;

    auto greenCircle = evilutionRegistry.create();
    Transform2DComponent& greenTransform = evilutionRegistry.emplace<Transform2DComponent>(greenCircle);
    RigidBody2dComponent& greenRigidbody = evilutionRegistry.emplace<RigidBody2dComponent>(greenCircle);
    RenderComponent& greenRender = evilutionRegistry.emplace<RenderComponent>(greenCircle);

    greenTransform.scale = glm::vec2{.05f};
    greenTransform.translation = {.7f, .0f};
    greenRender.color = glm::vec3{0.0f, 1.0f, 0.0f};
    greenRigidbody.velocity = glm::vec2{0.0f, .5f};
    greenRigidbody.mass = 0.25f;
    greenRender.model = circleModel;

    // create vector field

    for (int i = 0; i < gridCount; i++) {
        for (int j = 0; j < gridCount; j++) {
            auto vf = vec2FieldRegistry.create();
            Transform2DComponent& vfTransform = vec2FieldRegistry.emplace<Transform2DComponent>(vf);
            RenderComponent& vfRender = vec2FieldRegistry.emplace<RenderComponent>(vf);
            vec2FieldRegistry.emplace<RigidBody2dComponent>(vf);
            vfTransform.scale = glm::vec2(0.005f);
            vfTransform.translation = {
                -1.0f + (i + 0.5f) * 2.0f / gridCount,
                -1.0f + (j + 0.5f) * 2.0f / gridCount};
            vfRender.color = glm::vec3(1.0f);
            vfRender.model = squareModel;
        }
    }
}
} // namespace evilution