#version 330 core

// --- Inputs --- //

in vec2 uv;
in vec4 pos_to_light;

// --- Outputs --- //

out vec4 color;

// --- Textures --- //

layout(location = 0) uniform sampler2D tex_height;
layout(location = 1) uniform sampler2D tex_snow;
layout(location = 2) uniform sampler2D tex_dirt;
layout(location = 3) uniform sampler2D tex_shadow;

layout(location = 10) uniform sampler2D tex_grass_;
layout(location = 11) uniform sampler2D tex_sand_;
layout(location = 12) uniform sampler2D tex_rock_;
layout(location = 13) uniform sampler2D tex_snow_;
layout(location = 14) uniform sampler2D tex_rock_underwater_;

// --- Uniforms --- //

// light_params
uniform int shading_enable_phong;
uniform int shading_enable_shadow;
uniform vec3 shading_light_pos;
uniform vec3 shading_Ia, shading_Id;
uniform float shading_shadow_intensity;

// texture_params

uniform int texture_type;

uniform float snow_threshold;

uniform float sand_min_height;
uniform float sand_max_height;
uniform float grass_max_height;

uniform float sand_max_slope;
uniform float grass_max_slope;

uniform int grass_h_transition;
uniform int grass_s_transition;
uniform int sand_h_transition;
uniform int sand_s_transition;

uniform float water_height;

uniform float DX;
uniform float DY;

uniform float NEAR;
uniform float FAR;

const int NONE = 0;
const int SHADES = 1;
const int TEXTURE = 2;


uniform int mode;

const int NORMAL = 0;
const int ONLY_REFLECT = 1;
const int ILLUMINATE = 2;

void main() {

	float height = texture(tex_height, uv).rgb[0];
	float snow = texture(tex_snow, uv)[0];

	if (mode == ONLY_REFLECT && height < 0) {
		discard;
	}

	vec3 derivative_x = vec3(1, 0, 1000*texture(tex_height, uv + vec2(DX, 0))[0] - 1000*texture(tex_height, uv - vec2(DX, 0)*2*DX)[0]);
	vec3 derivative_y = vec3(0, 1, 1000*texture(tex_height, uv + vec2(0, DX))[0] - 1000*texture(tex_height, uv - vec2(0, DX)*2*DY)[0]);
	vec3 normal = normalize(cross(derivative_x, derivative_y));

	vec3 ambient;
	vec3 diffuse;

	if (texture_type == TEXTURE) {
	
		vec3 color_rock = texture(tex_rock_, uv * vec2(20)).rgb;
		vec3 color_sand = texture(tex_sand_, uv * vec2(80)).rgb;
		vec3 color_snow = texture(tex_snow_, uv * vec2(20)).rgb;
		vec3 color_grass = texture(tex_grass_, uv * vec2(20)).rgb;
		vec3 color_rock_underwater = texture(tex_rock_underwater_, uv * vec2(20)).rgb;

		if (snow > 0) {
			ambient = color_snow;
		} else {
   			ambient = color_rock;
		}
		
	} else if (texture_type == NONE) {
		ambient = vec3(1, 1, 1);
	} else {
		if (snow < snow_threshold){
			ambient = vec3(height + 0.5);
		} else {
			ambient = vec3(snow + 0.5, 0, 0);
		}
	}

	if (shading_enable_phong != 0) {
		ambient *= shading_Ia;
		diffuse = shading_Id * dot(normal, normalize(shading_light_pos));
	}

	vec3 color_unshadowed = ambient + diffuse;

	if (shading_enable_shadow != 0 && mode != ILLUMINATE) {
		vec2 uv_light = vec2((pos_to_light.x + 1) / 2, (pos_to_light.y + 1) / 2);
		float bias = 0.100;
		float visibility = 1.0;

		float shadow_z_b = texture(tex_shadow, uv_light)[0];
		float shadow_z_n = 2.0 * shadow_z_b - 1.0;
		//float shadow_z_e = 2.0 * NEAR * FAR / (FAR + NEAR - shadow_z_n * (FAR - NEAR));
		float shadow_z_e = (shadow_z_n * (FAR - NEAR) + FAR + NEAR)/(-2.0);

		float grid_z_b = pos_to_light.z;
		float grid_z_n = 2.0 * grid_z_b - 1.0;
		//float grid_z_e = 2.0 * NEAR * FAR / (FAR + NEAR - grid_z_n * (FAR - NEAR));
		float grid_z_e = (grid_z_n * (FAR - NEAR) + FAR + NEAR)/(-2.0);

		float diff = grid_z_e - shadow_z_e;

		//float depth_shadow = 39 * shadow_z_b - NEAR;
		//float depth_grid = 39 - grid_z_e;

		float depth_shadow = 40 * shadow_z_b - NEAR;
		float depth_grid = 39 - grid_z_e;

		float value = depth_grid - depth_shadow;

		if (value < 0) {
			color = vec4(1, 1 , 1, 1);
		} if (value < 1) {
			color = vec4(value, 0, 0, 1);
		} else if (value < 2) {
			color = vec4(0, value - 1, 0, 1);
		} else {
			color = vec4(0, 0, value - 2, 1);
		} 

		/*
		if(diff > bias){
			visibility = shading_shadow_intensity;
		}


		
		color = vec4(vec3(visibility) * color_unshadowed, 1.0);*/
	} else {
		color = vec4(color_unshadowed, 1.0);
	}
}
