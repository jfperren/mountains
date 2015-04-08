#version 330 core

uniform float noise_width;
uniform float noise_height;

//uniform vec2[noise_width][noise_height];

in vec2 uv;
out vec3 color;

float f(float t) {
	return 6 * pow(t, 5) - 15 * pow(t, 4) + 10 * pow(t, 3);
}

void main() {

	float x = uv[0];// * noise_width;
	float y = uv[1];// * noise_height;

	vec2 xy = vec2(x, y);

	vec2 g_s = vec2(0.5, 0.7);
	vec2 g_t = vec2(0.2, 0.1);
	vec2 g_u = vec2(0.6, 0.2);
	vec2 g_v = vec2(0.4, 0.5);

	vec2 a = xy;
	vec2 b = xy - vec2(1.0, 0.0);
	vec2 c = xy - vec2(0.0, 1.0);
	vec2 d = xy - vec2(1.0, 1.0);

	float s = dot(g_s, a);
	float t = dot(g_t, b);
	float u = dot(g_u, c);
	float v = dot(g_v, d);

	// Mixing
	float f_x = f(x);
	float f_y = f(y);

	float st = (1 - f_x) * s + f_x * t;
	float uv = (1 - f_x) * u + f_x * v;

	float noise = (1 - f_y) * st + f_y * uv;

	color = vec3(noise, noise, noise);
}