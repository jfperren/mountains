#version 430 core

// --- Inputs --- //

in vec2 uv;
in vec4 pos_to_light;

// --- Outputs --- //

out vec4 color;
out float fragment_depth;

// --- Textures --- //

layout(location = 0) uniform sampler2D tex_height;
layout(location = 1) uniform sampler2D tex_dirt;
layout(location = 2) uniform sampler2D tex_shadow;
/*layout(location = 1) uniform sampler2D tex_grass;
layout(location = 2) uniform sampler2D tex_sand;
layout(location = 3) uniform sampler2D tex_rock;
layout(location = 4) uniform sampler2D tex_snow;
layout(location = 5) uniform sampler2D tex_rock_underwater;*/

// --- Uniforms --- //

// light_params
uniform int shading_enable, shading_shadow_enable;
uniform float shading_shadow_intensity;
uniform vec3 shading_light_pos;
uniform vec3 shading_Ia, shading_Id;

// texture_params

uniform int texture_type;

uniform float sand_min_height;
uniform float sand_max_height;
uniform float grass_max_height;

uniform float sand_max_slope;
uniform float grass_max_slope;

uniform int grass_h_transition;
uniform int grass_s_transition;
uniform int sand_h_transition;
uniform int sand_s_transition;

uniform float DX;
uniform float DY;

uniform float far;
uniform float near;

const int NONE = 0;
const int SHADES = 1;
const int TEXTURE = 2;

uniform int mode;

const int NORMAL = 0;
const int ONLY_REFLECT = 1;
const int ILLUMINATE = 2;

// others
uniform float water_height;
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

float smooth_interpolate(float alpha, float factor, float threshold) {
	if (alpha < threshold) 
		return 1-exp(-factor * (threshold-alpha));
	else
		return 0;
}

void main() {

	float height = texture(tex_height, uv).rgb[0];
	float dirt = texture(tex_dirt, uv)[0];

	if (only_reflect == 1 && height < 0) {
		discard;
	}

	vec3 derivative_x = vec3(1, 0, 1000*texture(tex_height, uv + vec2(DX, 0))[0] - 1000*texture(tex_height, uv - vec2(DX, 0)*2*DX)[0]);
	vec3 derivative_y = vec3(0, 1, 1000*texture(tex_height, uv + vec2(0, DX))[0] - 1000*texture(tex_height, uv - vec2(0, DX)*2*DY)[0]);
	vec3 normal = normalize(cross(derivative_x, derivative_y));
	
	vec3 ambient;
	vec3 diffuse;

	if (texture_type == TEXTURE) {
	/*
		vec3 color_rock = texture(tex_rock, uv * vec2(20)).rgb;
		vec3 color_sand = texture(tex_sand, uv * vec2(80)).rgb;
		vec3 color_grass = texture(tex_grass, uv * vec2(20)).rgb;
		vec3 color_rock_underwater = texture(tex_rock_underwater, uv * vec2(20)).rgb;

		float slope = 1- dot(normal, vec3(0, 0, 1));

		float alpha_sand = smooth_interpolate(slope, sand_s_transition, sand_max_slope);
		float alpha_grass = smooth_interpolate(slope, grass_s_transition, grass_max_slope);

		vec3 texture_color;

		// Sand vs rock
		if (slope < sand_max_slope) {
			alpha_sand = smooth_interpolate(slope, 80, sand_max_slope);
		} else {
			alpha_sand = 1;
		}

		vec3 sand_and_rock = alpha_sand * color_sand + (1-alpha_sand) * color_rock;
		vec3 grass_and_rock = alpha_grass * color_grass + (1-alpha_grass) * color_rock;

		if (height < sand_min_height) {

			float gamma = smooth_interpolate(height, 40, sand_min_height);
			texture_color = gamma * color_rock_underwater + (1-gamma) * sand_and_rock;

		} else if (height < sand_max_height) {

			float gamma = smooth_interpolate(height, sand_h_transition, sand_max_height);
			texture_color = gamma * sand_and_rock + (1-gamma) * grass_and_rock;

		} else if (height < grass_max_height) {

			float gamma = smooth_interpolate(height, grass_h_transition, grass_max_height);
			texture_color = gamma * grass_and_rock + (1-gamma) * color_rock;

		} else {
			texture_color = color_rock;
		}

   		ambient = texture_color;
		*/
	} else if (texture_type == NONE) {
		ambient = vec3(1, 1, 1);
	} else {
		if (dirt == 0){
			ambient = vec3(height + 0.5);
		} else {
			ambient = vec3(dirt + 0.5, 0, 0);
		}
	}

	if (shading_enable != 0) {
		ambient *= shading_Ia;
		diffuse = shading_Id * dot(normal, normalize(shading_light_pos));
	}

	vec3 color_unshadowed = ambient + diffuse;

	if (shading_shadow_enable != 0 && mode != ILLUMINATE) {
		vec2 uv_light = vec2((pos_to_light.x + 1) / 2, (pos_to_light.y + 1) / 2);
		float bias = 0.100;
		float visibility = 1.0;

		float shadow_z = texture(tex_shadow, uv_light)[0];

		float shadow_depth = (-2*far*near)/(far -near)/(shadow_z - ((far-near)/(far+near)));
		float grid_depth = (-2*far*near)/(far -near)/(pos_to_light.z - ((far-near)/(far+near)));

		float depth = grid_depth - shadow_depth;

		//vec3 texture_color = color_unshadowed / (1 + depth * color_unshadowed * shading_shadow_intensity);
		if(1 - (shadow_z - pos_to_light.z) < bias){
			visibility = 0.0;
		}
		
		color = vec4(vec3(visibility) * color_unshadowed, 1.0);
	} else {
		color = vec4(color_unshadowed, 1.0);
	}
}
