#version 330 core

in vec2 uv;
in float water_depth;

out vec4 color;

uniform sampler2D tex_main;
uniform sampler2D tex_mirror;
uniform sampler2D tex_height;
uniform sampler2D tex_depth;
uniform sampler2D tex_normal_map;

uniform vec3 light_pos;
uniform vec3 Id;

uniform float far;
uniform float near;

uniform vec3 water_color;
uniform	float water_height;
uniform	float water_transparency;
uniform	float water_depth_alpha_factor;
uniform float water_depth_color_factor;
uniform float water_reflection_factor;
uniform float waves_speed;
uniform float waves_tile_factor;
uniform float waves_amplitude;

const float time_factor = 0.05;

void main() {
    
	// Get the size of the screen (same as size of tex_mirror)
    ivec2 texture_size = textureSize(tex_mirror, 0);

	// Get screen-wide uv coordinates
    float _u = gl_FragCoord[0] / texture_size[0];
    float _v_inv = 1-gl_FragCoord[1] / texture_size[1];
	float _v = gl_FragCoord[1] / texture_size[1];
	vec2 uv_screen = gl_FragCoord.xy / texture_size.xy;
	vec2 uv_screen_inv = vec2(0, 1) + vec2(1, -1) * uv_screen;

	// Compute the depth of the water
	float z_ = texture(tex_depth, vec2(_u, _v))[0];
	float grid_depth = (-2*far*near)/(far -near)/(z_ - ((far-near)/(far+near)));
	float water_depth = (-2*far*near)/(far -near)/(gl_FragCoord.z - ((far-near)/(far+near)));

	float depth = grid_depth - water_depth;

	// Check if we need to take it into account
	if (depth <= 0) {
		discard;
	}
    
	// Compute color & alpha according to depth
	vec3 texture_color = water_color / (1 + depth * water_depth_color_factor);
	float alpha = water_transparency * (1 + water_depth_alpha_factor * depth);

	// Get 
    vec3 mirror_color = texture(tex_mirror, uv_screen_inv).rgb;
    
    // Mix the two together
    color = vec4(mix(texture_color, mirror_color, vec3(water_reflection_factor)), alpha);
}