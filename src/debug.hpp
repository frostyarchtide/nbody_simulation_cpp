#pragma once

#include <string>

namespace debug {
    void print_info(std::string message, std::string header = std::string("INFO"));
    void print_warning(std::string message, std::string header = std::string("WARNING"));
    void print_error(std::string message, bool trap = false, std::string header = std::string("ERROR"));
    void print_glfw_error(int id, const char* description);
    
    void verify_shader_compilation(unsigned int shader, std::string name = "");
}
