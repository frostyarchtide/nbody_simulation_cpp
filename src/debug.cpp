#include <csignal>
#include <iostream>

#include "debug.hpp"

void debug::print_info(const char* message, const char* header) {
    std::cout
        << "\033[90;1m["
        << header
        << "]\033[0;90m "
        << message
        << "\033[0m"
        << std::endl;
}

void debug::print_warning(const char* message, const char* header) {
    std::cout
        << "\033[93;1m["
        << header
        << "]\033[0;93m "
        << message
        << "\033[0m"
        << std::endl;
}

void debug::print_error(const char* message, bool trap, const char* header) {
    std::cerr
        << "\033[91;1m["
        << header
        << "]\033[0;91m "
        << message
        << "\033[0m"
        << std::endl;

    if (trap)
        raise(SIGTRAP);
}

void debug::print_glfw_error(int id, const char* description) {
    debug::print_error(
        description,
        true,
        (std::string("GLFW ERROR ") + std::to_string(id)).c_str()
    );
}
