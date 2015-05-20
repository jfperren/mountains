#version 330 core

in vec2 uv;

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

uniform float time;

uniform float waves_speed;
uniform float waves_tile_factor;
uniform float waves_amplitude;

const float time_factor = 0.05;

uniform float waves_speed;
uniform float waves_tile_factor;
uniform float waves_amplitude;

// Speed
// Tile factor
// direction
// amplitude

const float time_factor = 0.05;

void main() {
    
	float depth = water_height - texture(tex_height, uv)[0];

	// Check if we need to take it into account
	if (depth <= 0) {
		discard;
	}

    // Get the size of the screen (same as size of tex_mirror)
    ivec2 texture_size = textureSize(tex_mirror, 0);
    
    // FragCoord gives pixel coordinates, transform it into uv coordinates by normalizing
    float _u = gl_FragCoord[0] / texture_size[0];
    float _v = 1-gl_FragCoord[1] / texture_size[1];
    
    // Get texture color w/ usual uv, mirror with computed _u/_v
    vec3 texture_color = water_color;

	texture_color = texture_color/(water_depth_color_factor * depth + 1);
	float alpha = water_transparency + (1 - water_transparency) * (water_depth_alpha_factor * depth);

    vec3 mirror_color = texture(tex_mirror, vec2(_u, _v)).rgb;
    
    vec3 ambiant = vec3(mix(texture_color, mirror_color, vec3(water_reflection_factor)));

	ambiant *= vec3(0.5, 0.5, 1);

	// Compute normal from the texture tex_normal_map
	vec3 normal = vec3(texture(tex_normal_map,  uv * vec2(waves_tile_factor)  + vec2(time * waves_speed))).rgb;
	normal[1] = 1;

	vec3 diffuse =  Id * (dot(normalize(normal), normalize(light_pos)));
	diffuse = diffuse * vec3(waves_amplitude);

	color = vec4(ambiant + diffuse, alpha);
}