#include "gravity_physics_system.hpp"
#include <iostream>

namespace evilution {

GravityPhysicsSystem::GravityPhysicsSystem(float strength) : strengthGravity{strength} {}


// dt stands for delta time, and specifies the amount of time to advance the simulation
// substeps is how many intervals to divide the forward time step in. More substeps result in a
// more stable simulation, but takes longer to compute
void GravityPhysicsSystem::update(entt::registry& registry, float dt, unsigned int substeps = 1) {
    const float stepDelta = dt / substeps;
    for (int i = 0; i < substeps; i++) {
        stepSimulation(registry, stepDelta);
    }
}

void GravityPhysicsSystem::stepSimulation(entt::registry& registry, float dt) {
    auto view = registry.view<Transform2DComponent, RigidBody2dComponent>();
    
    // Loops through all pairs of entities and applies attractive force between them
    // Only iterate through unique pairs
    for (auto it = view.begin(); it != view.end(); ++it) {
        auto entityA = *it;
        auto& transformA = view.get<Transform2DComponent>(entityA);
        auto& rigidbodyA = view.get<RigidBody2dComponent>(entityA);
        
        // Start from the next entity to avoid duplicate calculations
        auto next = it;
        ++next;
        for (; next != view.end(); ++next) {
            auto entityB = *next;
            auto& transformB = view.get<Transform2DComponent>(entityB);
            auto& rigidbodyB = view.get<RigidBody2dComponent>(entityB);
            
            auto force = computeForce(transformA, rigidbodyA, transformB, rigidbodyB);
            rigidbodyA.velocity += dt * -force / rigidbodyA.mass;
            rigidbodyB.velocity += dt * force / rigidbodyB.mass;
        }
    }

    // Position update pass
    for (auto entity : view) {
        auto& transform = view.get<Transform2DComponent>(entity);
        const auto& rigidbody = view.get<RigidBody2dComponent>(entity);
        transform.translation += dt * rigidbody.velocity;
    }
}

glm::vec2 GravityPhysicsSystem::computeForce(
    const Transform2DComponent& transformFrom,
    const RigidBody2dComponent& rigidbodyFrom,
    const Transform2DComponent& transformTo,
    const RigidBody2dComponent& rigidbodyTo) {
    
    glm::vec2 offset = transformFrom.translation - transformTo.translation;
    float distanceSquared = glm::dot(offset, offset);

    // Add minimum distance to prevent extreme forces at close range
    if (glm::abs(distanceSquared) < 1e-10f) {
      return {.0f, .0f};
    }
    
    float forceMagnitude = strengthGravity * rigidbodyFrom.mass * rigidbodyTo.mass / distanceSquared;
    
    return forceMagnitude * offset / glm::sqrt(distanceSquared);
}
}


