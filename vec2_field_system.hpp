#pragma once
#include "evilution_model.hpp"
#include "gravity_physics_system.hpp"
#include <entt/entt.hpp>
#include <memory>


namespace evilution {

class Vec2FieldSystem {

  public:
    static std::unique_ptr<EvilutionModel> createCircleModel(EvilutionDevice& device, unsigned int numSides);
    static std::unique_ptr<EvilutionModel> createSquareModel(EvilutionDevice& device, glm::vec2 offset);
    void update(const GravityPhysicsSystem& physicsSystem, entt::registry& registry);
};
} // namespace evilution