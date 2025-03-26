#include <chrono>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include "debug.hpp"

int main() {
    glfwSetErrorCallback(debug::print_glfw_error);

    if (!glfwInit())
        debug::print_error("GLFW failed to initialize", true);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    io.IniFilename = NULL;
    io.LogFilename = NULL;

    bool drawPoint = true;

    std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();
    
    while (!glfwWindowShouldClose(window)) {
        std::chrono::high_resolution_clock::time_point current_time = std::chrono::high_resolution_clock::now();
        float delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - last_time).count();

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (drawPoint)
            glDrawArrays(GL_POINTS, 0, 1);

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ 0, 0 });
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoTitleBar;

        ImGui::Begin("Debug", NULL, flags);
        ImGui::Text("%d", (unsigned int) (1.0f / delta_time + 0.5f));
        ImGui::Checkbox("Draw Point", &drawPoint);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        last_time = current_time;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteBuffers(1, &vertex_buffer);
    glfwDestroyWindow(window);
    glfwTerminate();
}
