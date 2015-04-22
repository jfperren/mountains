#version 330 core

in vec3 vertex_pos;
in vec2 vertex_tex_coord;

out vec2 uv;

uniform mat4 mvp;
uniform float water_height;

void main() {
    //vec3 pos_3d = vec3(vertex_pos.x, water_height, -vertex_pos.y);
	vec3 pos_3d = vertex_pos;
    gl_Position = mvp * vec4(pos_3d, 1.0);

	uv = vertex_tex_coord;
}