#pragma once

#include "evilution_components.hpp"
#include "evilution_model.hpp"
#include "marching_squares.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace evilution {
class MetaBallsSystem {
  public:


    MetaBallsSystem(EvilutionDevice& device, entt::registry& evilutionRegistry);

    void addMetaBall(const glm::vec2& center, const glm::vec2& velocity, float radius);
    float evaluateField(float x, float y) const;
    void updateMetaBalls(float deltaTime);
    void createInitialMesh();
    void updateMesh();
    void updateMarchingSquaresParameters(float threshold, int samplesX, int samplesY);


  private:
    static float implicitRectangle(float x, float y, float centerX, float centerY, float width, float height);
    static float implicitCircle(float x, float y, float centerX, float centerY, float radius);


    const float BOUNDS = .9f;
    entt::registry& evilutionRegistry;
    EvilutionDevice& evilutionDevice;
    std::vector<MetaBall> metaBalls;
    MarchingSquares marchingSquaresObject;
    std::shared_ptr<EvilutionModel>* modelPtr = nullptr;
    std::function<float(float, float)> metaBallField;
};
} // namespace evilution

