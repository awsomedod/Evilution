#include "first_app.hpp"

#include "simple_render_system.hpp"
#include "evilution_game_object.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <iostream>
namespace evilution {
 

class GravityPhysicsSystem2 {
 public:
  GravityPhysicsSystem2(float strength) : strengthGravity{strength} {}

  const float strengthGravity;

  // dt stands for delta time, and specifies the amount of time to advance the simulation
  // substeps is how many intervals to divide the forward time step in. More substeps result in a
  // more stable simulation, but takes longer to compute
  void update(std::vector<EvilutionGameObject>& objs, float dt, unsigned int substeps = 1) {
    const float stepDelta = dt / substeps;
    for (int i = 0; i < substeps; i++) {
      stepSimulation(objs, stepDelta);
    }
  }

  glm::vec2 computeForce(EvilutionGameObject& fromObj, EvilutionGameObject& toObj) const {
    auto offset = fromObj.transform2d.translation - toObj.transform2d.translation;
    float distanceSquared = glm::dot(offset, offset);
    static float minDistanceSquared = std::numeric_limits<float>::infinity();
    if (distanceSquared < minDistanceSquared) {
      minDistanceSquared = distanceSquared;
      std::cout << "distanceSquared: " << distanceSquared << std::endl;
    }

    // clown town - just going to return 0 if objects are too close together...
    if (glm::abs(distanceSquared) < 0.001) {
      return {.0f, .0f};
    }

    float force =
        strengthGravity * toObj.rigidBody2d.mass * fromObj.rigidBody2d.mass / distanceSquared;
    return force * offset / glm::sqrt(distanceSquared);
  }

 private:
  void stepSimulation(std::vector<EvilutionGameObject>& physicsObjs, float dt) {
    // Loops through all pairs of objects and applies attractive force between them
    for (auto iterA = physicsObjs.begin(); iterA != physicsObjs.end(); ++iterA) {
      auto& objA = *iterA;
      for (auto iterB = iterA; iterB != physicsObjs.end(); ++iterB) {
        if (iterA == iterB) continue;
        auto& objB = *iterB;

        auto force = computeForce(objA, objB);
        objA.rigidBody2d.velocity += dt * -force / objA.rigidBody2d.mass;
        objB.rigidBody2d.velocity += dt * force / objB.rigidBody2d.mass;
      }
    }

    // update each objects position based on its final velocity
    for (auto& obj : physicsObjs) {
      obj.transform2d.translation += dt * obj.rigidBody2d.velocity;
    }
  }
};

class Vec2FieldSystem {
 public:
  void update(
      const GravityPhysicsSystem2& physicsSystem,
      std::vector<EvilutionGameObject>& physicsObjs,
      std::vector<EvilutionGameObject>& vectorField) {
    // For each field line we caluclate the net graviation force for that point in space
    for (auto& vf : vectorField) {
      glm::vec2 direction{};
      for (auto& obj : physicsObjs) {
        direction += physicsSystem.computeForce(obj, vf);
      }

      // This scales the length of the field line based on the log of the length
      // values were chosen just through trial and error based on what i liked the look
      // of and then the field line is rotated to point in the direction of the field
      vf.transform2d.scale.x =
          0.005f + 0.045f * glm::clamp(glm::log(glm::length(direction) + 1) / 3.f, 0.f, 1.f);
      vf.transform2d.rotation = atan2(direction.y, direction.x);
    }
  }
};

std::unique_ptr<EvilutionModel> createSquareModel(EvilutionDevice& device, glm::vec2 offset) {
  std::vector<EvilutionModel::Vertex> vertices = {
      {{-0.5f, -0.5f}},
      {{0.5f, 0.5f}},
      {{-0.5f, 0.5f}},
      {{-0.5f, -0.5f}},
      {{0.5f, -0.5f}},
      {{0.5f, 0.5f}},  //
  };
  for (auto& v : vertices) {
    v.position += offset;
  }

  // Define indices for two triangles
    std::vector<uint32_t> indices = {
        0, 1, 2,  // first triangle
        3, 4, 5   // second triangle
    };
  return std::make_unique<EvilutionModel>(device, vertices, indices);
}

std::unique_ptr<EvilutionModel> createCircleModel(EvilutionDevice& device, unsigned int numSides) {
    std::vector<EvilutionModel::Vertex> uniqueVertices{};
    std::vector<uint32_t> indices{};

    // Create vertices
    for (int i = 0; i < numSides; i++) {
        float angle = i * glm::two_pi<float>() / numSides;
        uniqueVertices.push_back({{glm::cos(angle), glm::sin(angle)}});
    }
    uniqueVertices.push_back({}); // adds center vertex at 0, 0

    // Create indices for triangle fan
    const uint32_t centerIndex = numSides; // Index of center vertex
    for (uint32_t i = 0; i < numSides; i++) {
        indices.push_back(centerIndex);        // Center vertex
        indices.push_back(i);                  // Current vertex
        indices.push_back((i + 1) % numSides); // Next vertex (wraps around to 0)
    }

    return std::make_unique<EvilutionModel>(device, uniqueVertices, indices);
}

 FirstApp::FirstApp() {}
 
 FirstApp::~FirstApp() {}
 
 void FirstApp::run() {
  // create some models
  std::shared_ptr<EvilutionModel> squareModel = createSquareModel(
      evilutionDevice,
      {.5f, .0f});  // offset model by .5 so rotation occurs at edge rather than center of square
  std::shared_ptr<EvilutionModel> circleModel = createCircleModel(evilutionDevice, 64);

  // create physics objects
  std::vector<EvilutionGameObject> physicsObjects{};
  auto red = EvilutionGameObject::createGameObject();
  red.transform2d.scale = glm::vec2{.05f};
  red.transform2d.translation = {.5f, .0f};
  red.color = {1.f, 0.f, 0.f};
  red.rigidBody2d.velocity = {.0f, 2.0f};
  red.rigidBody2d.mass = 0.1f;
  red.model = circleModel;
  physicsObjects.push_back(std::move(red));
  auto blue = EvilutionGameObject::createGameObject();
  blue.transform2d.scale = glm::vec2{.05f};
  blue.transform2d.translation = {0.f, 0.f};
  blue.color = {0.f, 0.f, 1.f};
  blue.rigidBody2d.velocity = {.0f, .0f};
  blue.rigidBody2d.mass = 10.f;
  blue.model = circleModel;
  physicsObjects.push_back(std::move(blue));

  // create vector field
  std::vector<EvilutionGameObject> vectorField{};
  int gridCount = 40;
  for (int i = 0; i < gridCount; i++) {
    for (int j = 0; j < gridCount; j++) {
      auto vf = EvilutionGameObject::createGameObject();
      vf.transform2d.scale = glm::vec2(0.005f);
      vf.transform2d.translation = {
          -1.0f + (i + 0.5f) * 2.0f / gridCount,
          -1.0f + (j + 0.5f) * 2.0f / gridCount};
      vf.color = glm::vec3(1.0f);
      vf.model = squareModel;
      vectorField.push_back(std::move(vf));
    }
  }

  GravityPhysicsSystem2 gravitySystem{0.81f};
  Vec2FieldSystem vecFieldSystem{};

  SimpleRenderSystem simpleRenderSystem{evilutionDevice, evilutionRenderer.getSwapChainRenderPass()};
 
  while (!evilutionWindow.shouldClose()) {
    glfwPollEvents();
 
     if (auto commandBuffer = evilutionRenderer.beginFrame()) {
      // update systems
      gravitySystem.update(physicsObjects, 1.f / 60, 5);
      vecFieldSystem.update(gravitySystem, physicsObjects, vectorField);

      // render system
      evilutionRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, physicsObjects);
      simpleRenderSystem.renderGameObjects(commandBuffer, vectorField);
      evilutionRenderer.endSwapChainRenderPass(commandBuffer);
      evilutionRenderer.endFrame();
    }
  }
}
} // namespace evilution
