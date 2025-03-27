#include <cmath>
#include <string>
#include <time.h>

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

const glm::uvec2 RESOLUTION = glm::uvec2(800, 600);
const float ASPECT_RATIO = RESOLUTION.y / (float) RESOLUTION.x;
const unsigned int PARTICLE_COUNT = 2 << 12;
const unsigned int WORKGROUP_SIZE = 16;
const unsigned int FRAMERATE = 60;
const float DELTA_TIME = 1.0f / (float) FRAMERATE;
const unsigned int CIRCLE_VERTICES = 64;
const float CIRCLE_RADIUS = 0.01f;
const float G = 0.0005f;
const float DAMPENING = 0.01f;

typedef struct particle {
    glm::vec2 position;
    glm::vec2 velocity;
} particle;

int main() {
    srand(time(NULL));

    Window window(RESOLUTION.x, RESOLUTION.y, "N-Body Simulation");
    Recorder recorder(window, FRAMERATE, "recordings");

    float vertices[(CIRCLE_VERTICES + 2) * 2] = { 0.0f };
    for (unsigned int i = 0; i < CIRCLE_VERTICES; i++) {
        float angle = 2.0 * M_PI * i / (float) CIRCLE_VERTICES;
        vertices[(i + 1) * 2] = CIRCLE_RADIUS * cos(angle);
        vertices[(i + 1) * 2 + 1] = CIRCLE_RADIUS * sin(angle);
    }
    vertices[(CIRCLE_VERTICES + 1) * 2] = vertices[2];
    vertices[(CIRCLE_VERTICES + 1) * 2 + 1] = vertices[3];

    unsigned int vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    std::string vertex_shader_source =
        #include "shaders/shader.glsl.vert"
        ;

    vertex_shader_source.replace(
        vertex_shader_source.find("__PARTICLE_COUNT__"),
        std::string("__PARTICLE_COUNT__").length(),
        std::to_string(PARTICLE_COUNT)
    );
    vertex_shader_source.replace(
        vertex_shader_source.find("__CIRCLE_RADIUS__"),
        std::string("__CIRCLE_RADIUS__").length(),
        std::to_string(CIRCLE_RADIUS)
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
    compute_shader_source.replace(
        compute_shader_source.find("__G__"),
        std::string("__G__").length(),
        std::to_string(G)
    );
    compute_shader_source.replace(
        compute_shader_source.find("__DAMPENING__"),
        std::string("__DAMPENING__").length(),
        std::to_string(DAMPENING)
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
    if (PARTICLE_COUNT % WORKGROUP_SIZE != 0)
        debug::print_error(
            std::string("Particle count is not factorable by workgroup size (")
            + std::to_string(PARTICLE_COUNT)
            + " % "
            + std::to_string(WORKGROUP_SIZE)
            + " == "
            + std::to_string(PARTICLE_COUNT % WORKGROUP_SIZE)
            + ")",
            true
        );

    for (unsigned int i = 0; i < PARTICLE_COUNT; i++) {
        float angle = 2.0f * M_PI * (rand() / (float) RAND_MAX);
        float distance = rand() / (float) RAND_MAX;
        particles[i].position = glm::vec2(cos(angle), sin(angle)) * distance;
        particles[i].velocity =
            glm::rotate(glm::normalize(particles[i].position), M_PI_2f)
            * glm::sqrt(0.5f / distance);
    }

    unsigned int storage_buffer;
    glGenBuffers(1, &storage_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, storage_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, storage_buffer);
    
    glUseProgram(shader_program);
    glm::mat4 projection_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(ASPECT_RATIO, 1.0f, 1.0f));
    glUniformMatrix4fv(
        glGetUniformLocation(shader_program, "projection_matrix"),
        1,
        GL_FALSE,
        &projection_matrix[0][0]
    );

    float view_scale = 1.0f;
    bool paused = true;
    bool stepping = false;

    while (!window.should_close()) {
        glfwPollEvents();

        if (!paused || stepping) {
            glUseProgram(compute_program);
            glDispatchCompute(PARTICLE_COUNT / WORKGROUP_SIZE, 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        stepping = false;

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glm::mat4 view_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(view_scale));
        glUniformMatrix4fv(
            glGetUniformLocation(shader_program, "view_matrix"),
            1,
            GL_FALSE,
            &view_matrix[0][0]
        );

        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, CIRCLE_VERTICES + 2, PARTICLE_COUNT);
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
        ImGui::DragFloat("View Scale", &view_scale, 0.01f, 0.01f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::Checkbox("Paused", &paused);
        if (ImGui::Button("Step")) stepping = true;
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
