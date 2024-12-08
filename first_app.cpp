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
            vecFieldSystem.update(gravitySystem, evilutionRegistry);

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
    std::shared_ptr<EvilutionModel> squareModel = Vec2FieldSystem::createSquareModel(evilutionDevice, glm::vec2{.5f, .0f});

    auto redCircle = evilutionRegistry.create();
    Transform2DComponent& redTransform = evilutionRegistry.emplace<Transform2DComponent>(redCircle);
    RigidBody2dComponent& redRigidbody = evilutionRegistry.emplace<RigidBody2dComponent>(redCircle);
    RenderComponent& redRender = evilutionRegistry.emplace<RenderComponent>(redCircle);
    evilutionRegistry.emplace<PhysicsObjectComponent>(redCircle);
    
    redTransform.scale = glm::vec2{.05f};
    redTransform.translation = {.5f, .5f};
    redRender.color = glm::vec3{1.0f, 0.0f, 0.0f};
    redRigidbody.velocity = glm::vec2{-.5f, .0f};
    redRender.model = circleModel;

    auto blueCircle = evilutionRegistry.create();
    Transform2DComponent& blueTransform = evilutionRegistry.emplace<Transform2DComponent>(blueCircle);
    RigidBody2dComponent& blueRigidbody = evilutionRegistry.emplace<RigidBody2dComponent>(blueCircle);
    RenderComponent& blueRender = evilutionRegistry.emplace<RenderComponent>(blueCircle);
    evilutionRegistry.emplace<PhysicsObjectComponent>(blueCircle);

    blueTransform.scale = glm::vec2{.05f};
    blueTransform.translation = {-.45f, -.25f};
    blueRender.color = glm::vec3{0.0f, 0.0f, 1.0f};
    blueRigidbody.velocity = glm::vec2{.5f, .0f};
    blueRender.model = circleModel;

    // create vector field

    int gridCount = 10;
    for (int i = 0; i < gridCount; i++) {
        for (int j = 0; j < gridCount; j++) {
            auto vf = evilutionRegistry.create();
            Transform2DComponent& vfTransform = evilutionRegistry.emplace<Transform2DComponent>(vf);
            RenderComponent& vfRender = evilutionRegistry.emplace<RenderComponent>(vf);
            evilutionRegistry.emplace<RigidBody2dComponent>(vf);
            evilutionRegistry.emplace<Vec2FieldComponent>(vf);
            vfTransform.scale = glm::vec2(0.02f);
            vfTransform.translation = {
                -1.0f + (i + 0.5f) * 2.0f / gridCount,
                -1.0f + (j + 0.5f) * 2.0f / gridCount};
            vfRender.color = glm::vec3(1.0f);
            vfRender.model = squareModel;
        }
    }
}
} // namespace evilution