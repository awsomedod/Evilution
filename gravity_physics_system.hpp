#pragma once
#include "evilution_components.hpp"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace evilution {
class GravityPhysicsSystem {
  public:
    GravityPhysicsSystem(float strength);
    const float strengthGravity;
    void update(entt::registry& registry, float dt, unsigned int substeps);

    glm::vec2 computeForce(const Transform2DComponent& transformFrom, const RigidBody2dComponent& rigidbodyFrom,
                           const Transform2DComponent& transformTo, const RigidBody2dComponent& rigidbodyTo) const;

  private:
    void stepSimulation(entt::registry& registry, float dt);

};
} // namespace evilution
