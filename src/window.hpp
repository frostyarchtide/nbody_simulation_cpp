#pragma once

#include <chrono>
#include <string>

#include <GLFW/glfw3.h>

class Window {
public:
    Window(unsigned int width, unsigned int height, std::string title);
    ~Window();

    float get_delta_time() const;
    bool should_close() const;
    void swap_buffers();
    void update();

private:
    GLFWwindow* window;
    std::chrono::high_resolution_clock::time_point last_time;
    std::chrono::high_resolution_clock::time_point current_time;
    float delta_time;
};
