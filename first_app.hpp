#pragma once

#include "evilution_device.hpp"
#include "evilution_game_object.hpp"
#include "evilution_renderer.hpp"
#include "evilution_window.hpp"

// std
#include <vector>
namespace evilution {
class FirstApp {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

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
    std::vector<EvilutionGameObject> gameObjects;
};
} // namespace evilution