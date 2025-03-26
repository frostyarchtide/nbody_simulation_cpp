#include "debug.hpp"

#include <csignal>
#include <iostream>
#include <string.h>

#include <epoxy/gl.h>

void debug::print_info(std::string message, std::string header) {
    std::cout
        << "\033[90;1m["
        << header
        << "]\033[0;90m "
        << message
        << "\033[0m"
        << std::endl;
}

void debug::print_warning(std::string message, std::string header) {
    std::cout
        << "\033[93;1m["
        << header
        << "]\033[0;93m "
        << message
        << "\033[0m"
        << std::endl;
}

void debug::print_error(std::string message, bool trap, std::string header) {
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

void debug::verify_shader_compilation(unsigned int shader, std::string name) {
    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        if (!name.empty()) {
            debug::print_error(name + " " + std::string(info_log), true, "SHADER COMPILATION ERROR");
        } else {
            debug::print_error(info_log, true, "SHADER COMPILATION ERROR");
        }
    }
}
