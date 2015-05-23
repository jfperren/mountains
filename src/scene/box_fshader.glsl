#version 330 core

in vec3 fragment_pos;
in vec2 uv;

out vec4 color;

layout (location = 0) uniform sampler2D tex_height;
layout (location = 1) uniform sampler2D tex_water_depth;

uniform int grid_enable;

uniform int water_enable;
uniform vec3 water_color;
uniform	float water_height;
uniform	float water_transparency;
uniform	float water_depth_alpha_factor;
uniform float water_depth_color_factor;
uniform float water_reflection_factor;

void main() {

	if (grid_enable == 0) {
		if (fragment_pos[1] > 0) {
			discard;
		} else {
			color = vec4(0.1, 0.1, 0.1, 1.0);
		}
	} else {
		float grid_height = texture(tex_height, uv)[0];
		
		// Check if we need to take it into account
		if (fragment_pos[1] < grid_height) {
			// Solid base
			color = vec4(0.1, 0.1, 0.1, 1.0);
		} else if (fragment_pos[1] < water_height) {
			// Water
			if (water_enable == 0) {
				discard;
			} else {
				color = vec4(water_color, water_transparency);
			}
		} else {
			// Air
			discard;
		}
	}

	//texture_color = texture_color/(water_depth_color_factor * depth + 1);
	//float alpha = water_transparency + (1 - water_transparency) * (water_depth_alpha_factor * depth);

    // Mix the two together
    //color = vec4(0.5, 0.5, 1, 1);//vec4(mix(texture_color, mirror_color, vec3(water_reflection_factor)), alpha);
}