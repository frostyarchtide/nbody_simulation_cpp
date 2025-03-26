R"(
#version 430

struct particle {
    vec2 position;
    vec2 velocity;
};

const float PI = acos(-1.0);

layout(location = 0) in vec2 position;
layout(binding = 1, std430) readonly buffer StorageBuffer {
    particle particles[];
} storage_buffer;

void main() {
    vec2 offset = storage_buffer.particles[gl_InstanceID].position;
    gl_Position = vec4(position + offset, 0.0, 1.0);
}
)"
