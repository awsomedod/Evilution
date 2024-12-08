#pragma once

#include "evilution_device.hpp"
#include "evilution_renderer.hpp"
#include "evilution_window.hpp"

#include <entt/entt.hpp>

namespace evilution {
class FirstApp {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 800;

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
    entt::registry vec2FieldRegistry {};
};
} // namespace evilution