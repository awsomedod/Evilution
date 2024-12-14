#pragma once

#include "evilution_renderer.hpp"

// std
#include <entt/entt.hpp>

namespace evilution {
class FirstApp {
  public:
    static constexpr int WIDTH = 1000;
    static constexpr int HEIGHT = 1000;

    FirstApp();
    ~FirstApp();

    FirstApp(const FirstApp&) = delete;
    FirstApp& operator=(const FirstApp&) = delete;

    void run();

  private:
    void loadGameObjects();

    EvilutionWindow evilutionWindow{WIDTH, HEIGHT, "Evilution"};
    EvilutionDevice evilutionDevice{evilutionWindow};
    EvilutionRenderer evilutionRenderer{evilutionWindow, evilutionDevice};
    entt::registry evilutionRegistry {};
};
} // namespace evilution