#version 330 core
uniform mat4 mvp;

uniform sampler2D tex_height;

uniform int grid_enable;
uniform int grid_length;
uniform int grid_width;

in vec2 vertex_pos;

out float height;
out vec2 uv;

void main() {
    uv = vertex_pos + vec2(0.5, 0.5);

	height = texture(tex_height, uv).rgb[0];

	vec3 vertex_pos_3d;
	if (grid_enable == 1) {
		vertex_pos_3d = vec3(vertex_pos.x * grid_length, height, vertex_pos.y * grid_width);
	} else {
		vertex_pos_3d = vec3(vertex_pos.x * grid_length, 0, vertex_pos.y * grid_width);
	}

    gl_Position = mvp * vec4(vertex_pos_3d, 1.0);
}