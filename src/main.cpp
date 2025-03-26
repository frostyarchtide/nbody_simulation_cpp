#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include "debug.hpp"

int main() {
    glfwSetErrorCallback(debug::print_glfw_error);

    if (!glfwInit())
        debug::print_error("GLFW failed to initialize", true);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "N-Body Simulation", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        debug::print_error("GLFW failed to create window", true);
    }

    glfwMakeContextCurrent(window);

    debug::print_info((const char*) glGetString(GL_VERSION), "OPENGL VERSION");

    float vertices[] = {
        0.0f, 0.0f,
    };

    unsigned int vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_POINTS, 0, 1);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
