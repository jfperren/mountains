#version 330 core
out vec3 color;
in float height;
uniform sampler2D color1D;

void main() {
	//color = texture(color1D, vec2(height, 1.0)).rgb;
	color = vec3(height, height, height);
}
