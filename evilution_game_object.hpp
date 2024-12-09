#pragma once

#include "evilution_model.hpp"

// std
#include <memory>

namespace evilution {

struct RigidBody2dComponent {
  glm::vec2 velocity;
  float mass{1.0f};
};

struct Transform2dComponent {
  glm::vec2 translation{};  // (position offset)
  glm::vec2 scale{1.f, 1.f};
  float rotation;

  glm::mat2 mat2() {
    const float s = glm::sin(rotation);
    const float c = glm::cos(rotation);
    glm::mat2 rotMatrix{{c, s}, {-s, c}};

    glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
    return rotMatrix * scaleMat;
  }
};

class EvilutionGameObject {
 public:
  using id_t = unsigned int;

  static EvilutionGameObject createGameObject() {
    static id_t currentId = 0;
    return EvilutionGameObject{currentId++};
  }

  EvilutionGameObject(const EvilutionGameObject &) = delete;
  EvilutionGameObject &operator=(const EvilutionGameObject &) = delete;
  EvilutionGameObject(EvilutionGameObject &&) = default;
  EvilutionGameObject &operator=(EvilutionGameObject &&) = default;

  id_t getId() { return id; }

  std::shared_ptr<EvilutionModel> model{};
  glm::vec3 color{};
  Transform2dComponent transform2d{};
  RigidBody2dComponent rigidBody2d{};

 private:
  EvilutionGameObject(id_t objId) : id{objId} {}

  id_t id;
};
}  // namespace lve