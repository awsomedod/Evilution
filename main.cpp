#include "first_app.hpp"

// std
#include <cstdlib>
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;

    evilution::FirstApp app{};

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}