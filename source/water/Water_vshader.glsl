#version 330 core

in vec2 vertex_pos;

out vec2 uv;

uniform mat4 mvp;
uniform float water_height;

uniform int grid_width_in_chunks;
uniform int grid_length_in_chunks;

void main() {
    vec3 pos_3d = vec3(grid_length_in_chunks * vertex_pos.x, water_height, grid_width_in_chunks * vertex_pos.y);

    gl_Position = mvp * vec4(pos_3d, 1.0);

	uv = vertex_pos + vec2(0.5, 0.5);
}