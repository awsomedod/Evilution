#pragma once

#include "evilution_components.hpp"
#include "marching_squares.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace evilution {
class MetaBallsSystem {
  public:


    MetaBallsSystem(EvilutionDevice& device, entt::registry& evilutionRegistry) 
        : evilutionRegistry{evilutionRegistry}, evilutionDevice{device} {}

    void addMetaBall(const glm::vec2& center, const glm::vec2& velocity, float radius);
    float evaluateField(float x, float y) const;
    void updateMetaBalls(float deltaTime);
    void createInitialMesh();
    void updateMesh();


  private:
    static float implicitRectangle(float x, float y, float centerX, float centerY, float width, float height);
    static float implicitCircle(float x, float y, float centerX, float centerY, float radius);


    const float BOUNDS = .9f;
    entt::registry& evilutionRegistry;
    EvilutionDevice& evilutionDevice;
    entt::view<entt::get_t<MetaBall, RenderComponent>> metaBallView;
    MarchingSquares marchingSquaresObject {evilutionDevice, 1.f, 300, 300};
    entt::view<entt::get_t<RenderComponent>> renderView;
};
} // namespace evilution

