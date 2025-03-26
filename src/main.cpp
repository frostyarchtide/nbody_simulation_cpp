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
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
