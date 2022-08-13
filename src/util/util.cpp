#include "util.hpp"

void raiseError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
    glfwTerminate();
    exit(1);
}
