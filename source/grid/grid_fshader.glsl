#version 330 core

// --- Inputs --- //

in float height;
in vec2 uv;

// --- Outputs --- //

out vec4 color;

// --- Textures --- //

layout(location = 0) uniform sampler2D tex_height;
layout(location = 1) uniform sampler2D tex_grass;
layout(location = 2) uniform sampler2D tex_sand;
layout(location = 3) uniform sampler2D tex_rock;
layout(location = 4) uniform sampler2D tex_snow;
layout(location = 5) uniform sampler2D tex_rock_underwater;

// --- Uniforms --- //

// light_params
uniform vec3 light_pos;
uniform vec3 Ia, Id;

// texture_params
uniform float sand_min_level;
uniform float sand_max_level;
uniform float grass_max_level;

uniform float sand_max_slope;
uniform float grass_max_slope;

const float SNOW_MIN_LEVEL = 1.0;
const float SAND_MAX_LEVEL = 0.04;
const float GRASS_MAX_LEVEL = 0.8;
const float SAND_MIN_LEVEL = -0.1;

const float SAND_MAX_SLOPE = 0.8;
const float GRASS_MAX_SLOPE = 0.2;

// others
uniform float water_height;
const float pixel_unit = 1.0/2048.0;
uniform int only_reflect;

/** Computes the coefficient used as the 3rd arg of mix(3).
	The output ranges from 0 to 1 in the reals.
*/
float compute_linear_interpolation(float height1, float height2, float x) {
	float beta = (0 - 1) / (height1 - height2) * x + (height1 * 1 - height2 * 0) / (height1 - height2);
	if (beta > 0.99) {
		beta = 1;
	}
	if (beta < 0) {
		beta = 0;
	}

	return beta;
}

float clamp_height(float height) {
	float height_to_texture = height; 
	if (height >= 0.99){
		height_to_texture = 0.99;
	}
	else if (height <= -0.99) {
		height_to_texture = -0.99;
	}

	return height_to_texture;
}

float f(float alpha) {
	return min(1, pow(alpha/GRASS_MAX_SLOPE, 6));
}

float alpha_interval(float max, float min, float value){
	float sharpness = 0.1;

	if (value < min - sharpness) {
		return 0;
	} else if (value < min + sharpness) {
		return (value - (min - sharpness))/(2*sharpness);
	} else if (value < max - sharpness) {
		return 1;
	} else if (value < max + sharpness) {
		return 1 - (value - (max - sharpness))/(2*sharpness);
	} else {
		return 0;
	}
}

void main() {

	float height_to_texture = clamp_height(height);
	vec3 derivative_x = vec3(1, 0, 1000*texture(tex_height, uv + vec2(pixel_unit,0))[0] - 1000*texture(tex_height, uv - vec2(pixel_unit,0))[0]);
	vec3 derivative_y = vec3(0, 1, 1000*texture(tex_height, uv + vec2(0, pixel_unit))[0] - 1000*texture(tex_height, uv - vec2(0, pixel_unit))[0]);
	vec3 normal = normalize(cross(derivative_x, derivative_y));

	float alpha =  f(dot(normal, vec3(0, 0, 1)));

	vec3 color_rock = texture(tex_rock, uv * vec2(20)).rgb;
	vec3 color_sand = texture(tex_sand, uv * vec2(80)).rgb;
	vec3 color_grass = texture(tex_grass, uv * vec2(20)).rgb;
	vec3 color_rock_underwater = texture(tex_rock_underwater, uv * vec2(20)).rgb;

	vec3 texture_color;

	// Sand vs rock
	vec3 sand_and_rock = alpha * color_sand + (1-alpha) * color_rock;
	vec3 grass_and_rock = alpha * color_grass + (1-alpha) * color_rock;

	if (height < SAND_MIN_LEVEL) {
		float gamma = 1 - exp(10 * (height - SAND_MIN_LEVEL));

		texture_color = gamma * color_rock_underwater + (1-gamma) * sand_and_rock;
	} else if (height < SAND_MAX_LEVEL) {
		float gamma = 1 - exp(100 * (height - SAND_MAX_LEVEL));

		texture_color = gamma * sand_and_rock + (1-gamma) * grass_and_rock;
	} else if (height < GRASS_MAX_LEVEL) {
		float gamma = 1 - exp(10 * (height - GRASS_MAX_LEVEL));

		texture_color = gamma * grass_and_rock + (1-gamma) * color_rock;
	} else {
		texture_color = color_rock;
	}

   	vec3 ambient = Ia * texture_color;
  	vec3 diffuse = Id * dot(normal, normalize(light_pos));


	if (only_reflect == 1 && height < 0) {
		discard;
	} else {
		color = vec4(ambient + diffuse, 1.0);
	}
}
