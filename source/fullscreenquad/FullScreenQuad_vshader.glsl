#version 330 core

in vec3 vpoint;
out vec2 uv;

uniform int noise_width;
uniform int noise_height;

void main() {
    gl_Position = vec4(vpoint, 1.0);

    uv = ((vpoint + vec2(1.0, 1.0)) * 0.5).xy;
}