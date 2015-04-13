#version 330 core
out vec3 color;
in float height;
uniform sampler2D color1D;

void main() {
	color = vec3(height, height, height);
}
