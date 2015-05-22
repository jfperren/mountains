#version 330 core

// --- Inputs --- //

in vec2 uv;

// --- Outputs --- //

out vec4 color;

// --- Textures --- //

layout(location = 0) uniform sampler2D tex_height;
layout(location = 1) uniform sampler2D tex_snow;

layout(location = 10) uniform sampler2D tex_grass_;
layout(location = 11) uniform sampler2D tex_sand_;
layout(location = 12) uniform sampler2D tex_rock_;
layout(location = 13) uniform sampler2D tex_snow_;
layout(location = 14) uniform sampler2D tex_rock_underwater_;

// --- Uniforms --- //

// light_params
uniform int shading_enable;
uniform vec3 shading_light_pos;
uniform vec3 shading_Ia, shading_Id;

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

uniform float DX;
uniform float DY;

const int NONE = 0;
const int SHADES = 1;
const int TEXTURE = 2;

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
	float snow = texture(tex_snow, uv)[0];

	if (only_reflect == 1 && height < 0) {
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

	if (shading_enable != 0) {
		ambient *= shading_Ia;
		diffuse = shading_Id * dot(normal, normalize(shading_light_pos));
	}

	color = vec4(ambient + diffuse, 1.0);
	
}
