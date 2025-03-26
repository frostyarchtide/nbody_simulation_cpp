#include <cmath>
#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include "debug.hpp"
#include "math.hpp"
#include "window.hpp"

typedef struct particle {
    vec2 position;
    vec2 velocity;
} particle;

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
    debug::verify_shader_compilation(vertex_shader, "shader.glsl.vert");

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

    const char* compute_shader_source =
        #include "shaders/shader.glsl.comp"
        ;

    unsigned int compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_shader, 1, &compute_shader_source, nullptr);
    glCompileShader(compute_shader);
    debug::verify_shader_compilation(compute_shader, "shader.glsl.comp");

    unsigned int compute_program = glCreateProgram();
    glAttachShader(compute_program, compute_shader);
    glLinkProgram(compute_program);

    particle particles[2 << 10];
    for (unsigned int i = 0; i < sizeof(particles) / sizeof(particle); i++) {
        particles[i] = {
            {
                2.0f * (rand() / (float) RAND_MAX - 0.5f),
                2.0f * (rand() / (float) RAND_MAX - 0.5f),
            },
            { 0.0f, 0.0f }
        };
    }

    unsigned int storage_buffers[2];
    glGenBuffers(2, storage_buffers);
    for (unsigned int i = 0; i < 2; i++) {
        debug::print_info(std::to_string(storage_buffers[i]));
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, storage_buffers[i]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, storage_buffers[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, storage_buffers[1]);

    unsigned int new_buffer = 1;
    
    glUniform1ui(glGetUniformLocation(shader_program, "instances"), sizeof(particles) / sizeof(particle));
    glUniform1ui(glGetUniformLocation(compute_program, "instances"), sizeof(particles) / sizeof(particle));

    while (!window.should_close()) {
        glfwPollEvents();

        glUseProgram(compute_program);
        glDispatchCompute(sizeof(particles) / sizeof(particle), 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glBindBuffer(GL_COPY_READ_BUFFER, storage_buffers[new_buffer]);
        glBindBuffer(GL_COPY_WRITE_BUFFER, storage_buffers[1 - new_buffer]);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(particles));
        new_buffer = (new_buffer == 1) ? 0 : 1;
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, storage_buffers[1 - new_buffer]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, storage_buffers[new_buffer]);

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glDrawArraysInstanced(GL_POINTS, 0, 1, sizeof(particles) / sizeof(particle));

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
        ImGui::Text("%lu bodies", sizeof(particles) / sizeof(particle));
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.update();
    }

    glDeleteProgram(shader_program);
    glDeleteBuffers(1, &vertex_buffer);
}
