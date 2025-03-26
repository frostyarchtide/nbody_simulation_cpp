R"(
#version 330

const float PI = acos(-1.0);

layout(location = 0) in vec2 position;

void main() {
    float theta = gl_InstanceID / 1000.0 * 2.0 * PI;
    vec2 offset = vec2(gl_InstanceID * cos(theta) / 1000.0, gl_InstanceID * sin(theta) / 1000.0);
    gl_Position = vec4(position + offset, 0.0, 1.0);
}
)"
