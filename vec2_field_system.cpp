#include "vec2_field_system.hpp"

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
} // namespace evilution