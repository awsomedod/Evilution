#include "meta_ball_system.hpp"
#include "evilution_components.hpp"
#include "marching_squares.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace evilution {

MetaBallsSystem::MetaBallsSystem(EvilutionDevice& device, entt::registry& evilutionRegistry)
    : evilutionRegistry{evilutionRegistry}, evilutionDevice{device}, marchingSquaresObject{device, 1.f, 20, 20} {
    metaBallField = [this](float x, float y) { return evaluateField(x, y); };
}

float MetaBallsSystem::implicitRectangle(float x, float y, float centerX, float centerY, float width, float height) {
    /*
    Returns > 1 for points inside the rectangle
            = 1 for points on the rectangle's perimeter
            < 1 for points outside the rectangle

    The function uses the max distance ratio to any edge, inverted so that:
    - Inside points have values > 1
    - Boundary points = 1
    - Outside points < 1
    */
    float dx = std::abs(x - centerX);
    float dy = std::abs(y - centerY);

    float distanceRatioX = (width * 0.5f - dx) / (width * 0.5f);
    float distanceRatioY = (height * 0.5f - dy) / (height * 0.5f);

    // Use the minimum ratio to determine if we're inside/outside
    // Adding 1 to invert the relationship (inside > 1, outside < 1)
    return 2.0f - (1.0f / std::min(distanceRatioX, distanceRatioY));
}

float MetaBallsSystem::implicitCircle(float x, float y, float centerX, float centerY, float radius) {
    float dx = x - centerX;
    float dy = y - centerY;
    return radius / std::sqrt(dx * dx + dy * dy);
}

void MetaBallsSystem::updateMetaBalls(float deltaTime) {

    for (auto& ball : metaBalls) {

        // Update position
        ball.center += ball.velocity * deltaTime;

        // Bounce off boundaries
        if (std::abs(ball.center.x) > BOUNDS - ball.radius) {
            ball.velocity.x *= -1;
            ball.center.x = std::copysign(BOUNDS - ball.radius, ball.center.x);
        }
        if (std::abs(ball.center.y) > BOUNDS - ball.radius) {
            ball.velocity.y *= -1;
            ball.center.y = std::copysign(BOUNDS - ball.radius, ball.center.y);
        }
    }

    updateMesh();
}

float MetaBallsSystem::evaluateField(float x, float y) const {
    float sum = 0.0f;
    for (const auto& ball : metaBalls) {
        sum += implicitCircle(x, y, ball.center.x, ball.center.y, ball.radius);
    }
    return sum;
}

void MetaBallsSystem::addMetaBall(const glm::vec2& center, const glm::vec2& velocity, float radius) {
    metaBalls.push_back({center, velocity, radius});
}

void MetaBallsSystem::createInitialMesh() {
    auto marchedSquares = marchingSquaresObject.marchingSquares(metaBallField);

    // Create entity with mesh
    auto entity = evilutionRegistry.create();
    auto& transform = evilutionRegistry.emplace<Transform2DComponent>(entity);
    auto& render = evilutionRegistry.emplace<RenderComponent>(entity);

    render.color = glm::vec3{1.0f, 0.0f, 0.0f}; // Default to red
    render.model = marchedSquares;
    modelPtr = &render.model;
}

void MetaBallsSystem::updateMesh() {
    auto marchedSquares = marchingSquaresObject.marchingSquares(metaBallField);
    *modelPtr = marchedSquares;
}

void MetaBallsSystem::updateMarchingSquaresParameters(float threshold, int samplesX, int samplesY) {
    /*
    Recreates the marching squares object with new parameters and updates the mesh
    
    Args:
        threshold: The isovalue threshold for the marching squares algorithm
        samplesX: Number of sample points in the X direction
        samplesY: Number of sample points in the Y direction
    */
    marchingSquaresObject.~MarchingSquares();
    new (&marchingSquaresObject) MarchingSquares{evilutionDevice, threshold, samplesX, samplesY};
    createInitialMesh();
    updateMesh();  // Update the mesh with the new parameters
}
} // namespace evilution