#version 330 core

in vec2 uv;
out vec3 color;

uniform float noise_width;
uniform float noise_height;

uniform float min_height;
uniform float max_height;

uniform int noise_type;

const int RANDOM_NOISE = 0;
const int PERLIN_NOISE = 1;

float f(float t) {
	return 6 * pow(t, 5) - 15 * pow(t, 4) + 10 * pow(t, 3);
}

float random(vec2 xy, float seed) {
	return fract(sin(dot(vec2(xy[0] + seed,xy[1] - seed), vec2(12.9898,78.233))) * 43758.5453);
}

float clampToInterval(float value, float current_min, float current_max, float expected_min, float expected_max) {
	return (value - current_min) / (current_max - current_min) * (expected_max - expected_min) + expected_min;
}

void main() {
	
	if (noise_type == RANDOM_NOISE) {

		float x = uv[0] * noise_width;
		float y = uv[1] * noise_height;

		x = floor(x);
		y = floor(y);

		// Pseudorandom
		float noise = random(vec2(x, y), 0);

		// Clamping [0, 1] -> [min, max]
		noise = clampToInterval(noise, 0, 1, min_height, max_height);

		//color = vec3(random, random, random);
		color = vec3(noise, noise, noise);

	} else if (noise_type == PERLIN_NOISE) {
		float x = uv[0] * noise_width;
		float y = uv[1] * noise_height;

		// Coordinate inside the cell
		float x_in_cell = fract(x);
		float y_in_cell = fract(y);

		// Coordinate of the cell (bottom left corner)
		float x_of_cell = x - x_in_cell;
		float y_of_cell = y - y_in_cell;

		// Get corner coordinates
		vec2 bottom_left = vec2(x_of_cell, y_of_cell);
		vec2 bottom_right = vec2(x_of_cell + 1, y_of_cell);
		vec2 top_left = vec2(x_of_cell, y_of_cell + 1);
		vec2 top_right = vec2(x_of_cell + 1, y_of_cell + 1);

		vec2 g_s = vec2(random(bottom_left, 0) - 0.5, random(bottom_left, 0.11) - 0.5);
		vec2 g_t = vec2(random(bottom_right, 0) - 0.5, random(bottom_right, 0.11) - 0.5);
		vec2 g_u = vec2(random(top_left, 0) - 0.5, random(top_left, 0.11) - 0.5);
		vec2 g_v = vec2(random(top_right, 0) - 0.5, random(top_right, 0.11) - 0.5);

		vec2 a = vec2(x_in_cell, y_in_cell);
		vec2 b = vec2(x_in_cell - 1, y_in_cell);
		vec2 c = vec2(x_in_cell, y_in_cell - 1);
		vec2 d = vec2(x_in_cell - 1, y_in_cell - 1);

		float s = dot(g_s, a);
		float t = dot(g_t, b);
		float u = dot(g_u, c);
		float v = dot(g_v, d);

		// Mixing
		float f_x = f(x_in_cell);
		float f_y = f(y_in_cell);

		float st = (1 - f_x) * s + f_x * t;
		float uv = (1 - f_x) * u + f_x * v;

		float noise = (1 - f_y) * st + f_y * uv;

		// Clamp [- 4, 4] -> [min, max]
		noise = clampToInterval(noise, -0.3, 0.3, min_height, max_height);

		color = vec3(noise, noise, noise);
	} else {
		// Unknown, no noise
		color = vec3(uv[0], uv[0], uv[0]);
	}
}