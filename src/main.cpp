#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include "debug.hpp"
#include "window.hpp"

int main() {
    Window window(800, 600, "N-Body Simulation");

    float vertices[] = {
        0.0f, 0.0f,
    };

    unsigned int vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const char* vertex_shader_source =
        #include "shaders/shader.glsl.vert"
        ;

    const char* fragment_shader_source =
        #include "shaders/shader.glsl.frag"
        ;

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    debug::verify_shader_compilation(vertex_shader);

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    debug::verify_shader_compilation(fragment_shader, "shader.glsl.frag");

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);

    glUseProgram(shader_program);
    
    while (!window.should_close()) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArraysInstanced(GL_POINTS, 0, 1, 1000);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ 0, 0 });
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoInputs
            | ImGuiWindowFlags_NoTitleBar;

        ImGui::Begin("Debug", NULL, flags);
        ImGui::Text("%d fps", (int) (1.0f / window.get_delta_time() + 0.5f));
        ImGui::Text("%.2f ms", window.get_delta_time() * 1000.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.update();
    }

    glDeleteProgram(shader_program);
    glDeleteBuffers(1, &vertex_buffer);
}
