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

const float G = 1.0e-3;

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

    vec2 velocities[2 << 9] = { 0.0f, 0.0f };
    vec2 points[2 << 9];
    for (unsigned int i = 0; i < sizeof(points) / sizeof(vec2); i++) {
        points[i] = {
            2.0f * (rand() / (float) RAND_MAX - 0.5f),
            2.0f * (rand() / (float) RAND_MAX - 0.5f),
        };
    }

    unsigned int storage_buffer;
    glGenBuffers(1, &storage_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, storage_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, storage_buffer);
    
    glUniform1ui(glGetUniformLocation(shader_program, "instances"), sizeof(points) / sizeof(vec2));

    while (!window.should_close()) {
        glfwPollEvents();

        for (unsigned int i = 0; i < sizeof(points) / sizeof(vec2); i++) {
            for (unsigned int j = i; j < sizeof(points) / sizeof(vec2); j++) {
                float distance_squared =
                    pow(points[j].x - points[i].x, 2.0)
                    + pow(points[j].y - points[i].y, 2.0);

                if (distance_squared != 0.0) {
                    float force = 1.0 / distance_squared;
                    vec2 direction = (points[j] - points[i]).normalized();
                    velocities[i] += direction * force * window.get_delta_time() * G;
                    velocities[j] -= direction * force * window.get_delta_time() * G;
                }
            }
        }

        for (unsigned int i = 0; i < sizeof(points) / sizeof(vec2); i++) {
            points[i] += velocities[i] * window.get_delta_time();
        }

        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArraysInstanced(GL_POINTS, 0, 1, sizeof(points) / sizeof(vec2));

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
        ImGui::Text("%lu bodies", sizeof(points) / sizeof(vec2));
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.update();
    }

    glDeleteProgram(shader_program);
    glDeleteBuffers(1, &vertex_buffer);
}
