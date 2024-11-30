#include "evilution_window.hpp"

#include <stdexcept>

namespace evilution {
EvilutionWindow::EvilutionWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
        initWindow();
}
EvilutionWindow::~EvilutionWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
}

void EvilutionWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}

void EvilutionWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
                throw std::runtime_error("failed to create window surface!");
        }
}
} // namespace evilution