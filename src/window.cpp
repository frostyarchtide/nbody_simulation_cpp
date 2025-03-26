#include "window.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "debug.hpp"

Window::Window(unsigned int width, unsigned int height, std::string title) {
    glfwSetErrorCallback(debug::print_glfw_error);

    if (!glfwInit())
        debug::print_error("GLFW failed to initialize", true);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(800, 600, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        debug::print_error("GLFW failed to create window", true);
    }

    glfwMakeContextCurrent(window);
    debug::print_info((const char*) glGetString(GL_VERSION), "OPENGL VERSION");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    io.IniFilename = NULL;
    io.LogFilename = NULL;
}

Window::~Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window)
        glfwDestroyWindow(window);

    glfwTerminate();
}

bool Window::should_close() const {
    return glfwWindowShouldClose(window);
}

glm::uvec2 Window::get_resolution() const {
    glm::uvec2 resolution;
    glfwGetWindowSize(window, (int*) &resolution.x, (int*) &resolution.y);
    return resolution;
}

void Window::swap_buffers() {
    glfwSwapBuffers(window);
}

void Window::update() {
    swap_buffers();
}
