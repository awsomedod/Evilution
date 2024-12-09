#include "vec2_field_system.hpp"
#include "evilution_components.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>

namespace evilution {

std::unique_ptr<EvilutionModel> Vec2FieldSystem::createCircleModel(EvilutionDevice& device, unsigned int numSides) {
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

std::unique_ptr<EvilutionModel> Vec2FieldSystem::createSquareModel(EvilutionDevice& device, glm::vec2 offset) {
    std::vector<EvilutionModel::Vertex> vertices = {
        {{-0.5f, -0.5f}}, {{0.5f, 0.5f}}, {{-0.5f, 0.5f}}, {{-0.5f, -0.5f}}, {{0.5f, -0.5f}}, {{0.5f, 0.5f}}, //
    };
    for (auto& v : vertices) {
        v.position += offset;
    }

    // Define indices for two triangles
    std::vector<uint32_t> indices = {
        0, 1, 2, // first triangle
        3, 4, 5  // second triangle
    };
    return std::make_unique<EvilutionModel>(device, vertices, indices);
}

void Vec2FieldSystem::update(const GravityPhysicsSystem& physicsSystem, entt::registry& registry, entt::registry& vec2FieldRegistry) {
    // For each field line we caluclate the net graviation force for that point in space
    auto vfView = vec2FieldRegistry.view<Transform2DComponent, RigidBody2dComponent>();
    auto physicsObjsView = registry.view<Transform2DComponent, RigidBody2dComponent>();
    for (auto& vf : vfView) {
        glm::vec2 direction{};
        auto& vfTransform = vfView.get<Transform2DComponent>(vf);
        auto& vfRigidbody = vfView.get<RigidBody2dComponent>(vf);
        for (auto& obj : physicsObjsView) {
            auto& objTransform = physicsObjsView.get<Transform2DComponent>(obj);
            auto& objRigidbody = physicsObjsView.get<RigidBody2dComponent>(obj);
            direction += physicsSystem.computeForce(objTransform, objRigidbody, vfTransform, vfRigidbody);
        }

        // This scales the length of the field line based on the log of the length
        // values were chosen just through trial and error based on what i liked the look
        // of and then the field line is rotated to point in the direction of the field
        vfTransform.scale.x = 0.005f + 0.045f * glm::clamp(glm::log(glm::length(direction) + 1) / 3.f, 0.f, 1.f);
        vfTransform.rotation = atan2(direction.y, direction.x);
    }
}
} // namespace evilution