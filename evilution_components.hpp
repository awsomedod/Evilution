#pragma once

#include "evilution_model.hpp"

// std
#include <memory>

namespace evilution {

struct Transform2DComponent {
    glm::vec2 translation{};
    glm::vec2 scale{1.0f, 1.0f};
    float rotation{};

    glm::mat2 mat2() const {
        const float s = glm::sin(rotation);
        const float c = glm::cos(rotation);
        glm::mat2 rotMat{{c, s}, {-s, c}};
        glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};
        return rotMat * scaleMat;
    }
};

struct RenderComponent {
    std::shared_ptr<EvilutionModel> model;
    glm::vec3 color{};
};

struct RigidBody2dComponent {
    glm::vec2 velocity;
    float mass{1.0f};
};
} // namespace evilution