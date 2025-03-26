#pragma once

namespace debug {
    void print_info(const char* message, const char* header = "INFO");
    void print_warning(const char* message, const char* header = "WARNING");
    void print_error(const char* message, bool trap = false, const char* header = "ERROR");
    void print_glfw_error(int id, const char* description);
}
