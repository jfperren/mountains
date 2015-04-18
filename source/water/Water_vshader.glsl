#version 330 core

uniform mat4 mvp;
uniform float height;

in vec2 position;

void main() {
    vec3 pos_3d = vec3(position.x, height, -position.y);

    gl_Position = mvp * vec4(pos_3d, 1.0);
}