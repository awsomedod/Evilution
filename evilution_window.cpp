#include "evilution_window.hpp"

//std
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void EvilutionWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void EvilutionWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto evilutionWindow = reinterpret_cast<EvilutionWindow*>(glfwGetWindowUserPointer(window));
    evilutionWindow->framebufferResized = true;
    evilutionWindow->width = width;
    evilutionWindow->height = height;
}
} // namespace evilution