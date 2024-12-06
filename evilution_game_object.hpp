#pragma once

#include "evilution_model.hpp"

// std
#include <memory>

namespace evilution {

struct Transform2dComponent {
    glm::vec2 translation{};
    glm::vec2 scale{1.0f, 1.0f};
    float rotation;

    glm::mat2 mat2() {
        const float s = glm::sin(rotation);
        const float c = glm::cos(rotation);

        glm::mat2 rotationMat = {{c, s}, {-s, c}};

        glm::mat2 scaleMat = {{scale.x, 0.0f}, {0.0f, scale.y}};
        return rotationMat * scaleMat;
    }
};

class EvilutionGameObject {
  public:
    using id_t = unsigned int;

    static EvilutionGameObject createGameObject() {
        static id_t current_id = 0;
        return EvilutionGameObject{current_id++};
    }

    EvilutionGameObject(const EvilutionGameObject&) = delete;
    EvilutionGameObject& operator=(const EvilutionGameObject&) = delete;
    EvilutionGameObject(EvilutionGameObject&&) = default;
    EvilutionGameObject& operator=(EvilutionGameObject&&) = default;

    id_t get_id() const { return id; }

    std::shared_ptr<EvilutionModel> model{};
    glm::vec3 color{};
    Transform2dComponent transform2d{};

  private:
    EvilutionGameObject(id_t objId) : id{objId} {}
    id_t id;
};
} // namespace evilution