#include <cmath>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "debug.hpp"
#include "recorder.hpp"
#include "window.hpp"

const unsigned int PARTICLE_COUNT = 2 << 14;
const unsigned int WORKGROUP_SIZE = 256;
const unsigned int FRAMERATE = 60;
const float DELTA_TIME = 1.0f / (float) FRAMERATE;

typedef struct particle {
    glm::vec2 position;
    glm::vec2 velocity;
} particle;

int main() {
    Window window(800, 600, "N-Body Simulation");
    Recorder recorder(window, FRAMERATE, "recordings");

    float vertices[] = {
        0.0f, 0.0f,
    };

    unsigned int vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    std::string vertex_shader_source =
        #include "shaders/shader.glsl.vert"
        ;

    vertex_shader_source.replace(
        vertex_shader_source.find("__PARTICLE_COUNT__"),
        std::string("__PARTICLE_COUNT__").length(),
        std::to_string(PARTICLE_COUNT)
    );
    const char* vertex_shader_source_c_str = vertex_shader_source.c_str();

    const char* fragment_shader_source =
        #include "shaders/shader.glsl.frag"
        ;

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source_c_str, nullptr);
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

    std::string compute_shader_source =
        #include "shaders/shader.glsl.comp"
        ;

    compute_shader_source.replace(
        compute_shader_source.find("__WORKGROUP_SIZE__"),
        std::string("__WORKGROUP_SIZE__").length(),
        std::to_string(WORKGROUP_SIZE)
    );
    compute_shader_source.replace(
        compute_shader_source.find("__PARTICLE_COUNT__"),
        std::string("__PARTICLE_COUNT__").length(),
        std::to_string(PARTICLE_COUNT)
    );
    compute_shader_source.replace(
        compute_shader_source.find("__DELTA_TIME__"),
        std::string("__DELTA_TIME__").length(),
        std::to_string(DELTA_TIME)
    );
    const char* compute_shader_source_c_str = compute_shader_source.c_str();

    unsigned int compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_shader, 1, &compute_shader_source_c_str, nullptr);
    glCompileShader(compute_shader);
    debug::verify_shader_compilation(compute_shader, "shader.glsl.comp");

    unsigned int compute_program = glCreateProgram();
    glAttachShader(compute_program, compute_shader);
    glLinkProgram(compute_program);

    particle particles[PARTICLE_COUNT];
    if ((sizeof(particles) / sizeof(particle)) % WORKGROUP_SIZE != 0)
        debug::print_error(
            std::string("Particle count is not factorable by workgroup size (")
            + std::to_string(sizeof(particles) / sizeof(particle))
            + " % "
            + std::to_string(WORKGROUP_SIZE)
            + " == "
            + std::to_string((sizeof(particles) / sizeof(particle)) % WORKGROUP_SIZE)
            + ")",
            true
        );

    for (unsigned int i = 0; i < sizeof(particles) / sizeof(particle); i++) {
        float angle = 2.0f * M_PI * (rand() / (float) RAND_MAX);
        float distance = rand() / (float) RAND_MAX;
        particles[i].position = glm::vec2(cos(angle), sin(angle)) * distance;
        particles[i].velocity = glm::rotate(glm::normalize(particles[i].position), M_PI_2f) * glm::sqrt(0.05f / distance);
    }

    unsigned int storage_buffer;
    glGenBuffers(1, &storage_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, storage_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, storage_buffer);
    
    glm::mat4 view_matrix = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f);
    float view_scale = 100.0f;

    while (!window.should_close()) {
        glfwPollEvents();

        glUseProgram(compute_program);
        glDispatchCompute(sizeof(particles) / sizeof(particle) / WORKGROUP_SIZE, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glm::mat4 new_view_matrix = glm::scale(view_matrix, glm::vec3(view_scale));
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "view_matrix"), 1, GL_FALSE, &new_view_matrix[0][0]);

        glDrawArraysInstanced(GL_POINTS, 0, 1, sizeof(particles) / sizeof(particle));
        recorder.update();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ 0, 0 });
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoTitleBar;

        ImGui::Begin("Debug", NULL, flags);
        ImGui::DragFloat("View Scale", &view_scale, 1.0f, 0.0f, 1000.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
        if (ImGui::Button("Start Recording")) recorder.start_recording();
        if (ImGui::Button("Stop Recording")) recorder.stop_recording();
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.update();
    }

    glDeleteProgram(shader_program);
    glDeleteBuffers(1, &vertex_buffer);
}
