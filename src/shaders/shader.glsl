R"(
#version 430

struct particle {
    vec2 position;
    vec2 velocity;
};

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0) buffer StorageBuffer {
    particle particles[];
} storage_buffer;

layout(binding = 1) buffer NewStorageBuffer {
    particle particles[];
} new_storage_buffer;

uniform uint instances;

void main() {
    uint id = gl_GlobalInvocationID.x;
    for (uint i = id; i < instances; i++) {
        
    }
}
)"
