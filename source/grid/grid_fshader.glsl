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
uniform float sand_min_height;
uniform float sand_max_height;
uniform float grass_max_height;

uniform float sand_max_slope;
uniform float grass_max_slope;

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

float smooth_interpolate(float alpha, float factor, float threshold) {
	if (alpha < threshold) 
		return 1-exp(-factor * (threshold-alpha));
	else
		return 0;
}

void main() {

	float height_to_texture = clamp_height(height);
	vec3 derivative_x = vec3(1, 0, 1000*texture(tex_height, uv + vec2(pixel_unit,0))[0] - 1000*texture(tex_height, uv - vec2(pixel_unit,0))[0]);
	vec3 derivative_y = vec3(0, 1, 1000*texture(tex_height, uv + vec2(0, pixel_unit))[0] - 1000*texture(tex_height, uv - vec2(0, pixel_unit))[0]);
	vec3 normal = normalize(cross(derivative_x, derivative_y));

	vec3 color_rock = texture(tex_rock, uv * vec2(20)).rgb;
	vec3 color_sand = texture(tex_sand, uv * vec2(80)).rgb;
	vec3 color_grass = texture(tex_grass, uv * vec2(20)).rgb;
	vec3 color_rock_underwater = texture(tex_rock_underwater, uv * vec2(20)).rgb;


	float slope = 1- dot(normal, vec3(0, 0, 1));

	float alpha_sand = smooth_interpolate(slope, 20, sand_max_slope);
	float alpha_grass = smooth_interpolate(slope, 80, grass_max_slope);


	vec3 texture_color;

	// Sand vs rock
	/*if (slope < sand_max_slope) {
		alpha_sand = smooth_interpolate(slope, 80, sand_max_slope);
	} else {
		alpha_sand = 1;
	}*/

	vec3 sand_and_rock = alpha_sand * color_sand + (1-alpha_sand) * color_rock;
	vec3 grass_and_rock = alpha_grass * color_grass + (1-alpha_grass) * color_rock;

	if (height < sand_min_height) {

		float gamma = smooth_interpolate(height, 40, sand_min_height);
		texture_color = gamma * color_rock_underwater + (1-gamma) * sand_and_rock;

	} else if (height < sand_max_height) {

		float gamma = smooth_interpolate(height, 40, sand_max_height);
		texture_color = gamma * sand_and_rock + (1-gamma) * grass_and_rock;

	} else if (height < grass_max_height) {

		float gamma = smooth_interpolate(height, 10, grass_max_height);
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
