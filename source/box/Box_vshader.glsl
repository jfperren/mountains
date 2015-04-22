#version 330 core

in vec3 vertex_pos;
in vec2 vertex_tex_coord;

out vec3 fragment_pos;

uniform mat4 mvp;

void main() {
	vec3 pos = vertex_pos; 
    gl_Position = mvp * vec4(pos , 1.0);

	fragment_pos = vertex_pos;
}