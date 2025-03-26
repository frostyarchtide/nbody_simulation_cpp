R"(
#version 430

const float PI = acos(-1.0);

layout(location = 0) in vec2 position;
layout(binding = 0, std430) readonly buffer StorageBuffer {
    vec2 points[];
} storage_buffer;

uniform uint instances;

void main() {
    vec2 offset = storage_buffer.points[gl_InstanceID];
    gl_Position = vec4(position + offset, 0.0, 1.0);
}
)"
