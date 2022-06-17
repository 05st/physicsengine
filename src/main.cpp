#include "Application.hpp"

#include <iostream>

int main() {
    Application app;

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}