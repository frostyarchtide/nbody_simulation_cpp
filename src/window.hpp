#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Window {
public:
    Window(unsigned int width, unsigned int height, std::string title);
    ~Window();

    bool should_close() const;
    glm::uvec2 get_resolution() const;
    void swap_buffers();
    void update();

private:
    GLFWwindow* window;
};
