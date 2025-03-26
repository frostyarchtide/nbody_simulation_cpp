R"(
#version 430

#define PARTICLE_COUNT __PARTICLE_COUNT__
#define CIRCLE_RADIUS __CIRCLE_RADIUS__

struct particle {
    vec2 position;
    vec2 velocity;
};

const float PI = acos(-1.0);

layout(location = 0) in vec2 position;
layout(binding = 0, std430) readonly buffer StorageBuffer {
    particle particles[];
} storage_buffer;

out vec4 vertex_color;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main() {
    vec4 offset = view_matrix * projection_matrix * vec4(storage_buffer.particles[gl_InstanceID].position, 0.0, 1.0);
    gl_Position = view_matrix * projection_matrix * vec4(position, 0.0, 1.0) + offset;

    float alpha = 1.0 - length(position) / CIRCLE_RADIUS;
    float speed_factor = length(storage_buffer.particles[gl_InstanceID].velocity) / 1.0;
    vec4 color = mix(vec4(0.0, 0.25, 1.0, 0.0), vec4(1.0, 0.25, 0.0, 1.0), speed_factor);
    color.a = mix(0.0, color.a, alpha);
    vertex_color = color;
}
)"
