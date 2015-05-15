#version 330 core

in vec3 vertex_pos;

out vec3 fragment_pos;
out vec2 uv;

uniform int grid_length_in_chunks;
uniform int grid_width_in_chunks;

uniform float fractal_amplitude;
uniform float fractal_offset;

uniform mat4 mvp;

void main() {
	
	uv = (vec2(vertex_pos[0] + 1.0, vertex_pos[2] + 1.0)) * 0.5;

	vec3 pos = vertex_pos * vec3(grid_length_in_chunks, 1, grid_width_in_chunks); 
	pos = (pos * vec3(1, fractal_amplitude, 1)) + vec3(0, fractal_offset, 0);
    gl_Position = mvp * vec4(pos , 1.0);

	fragment_pos = pos;
}