#version 330 core

in vec3 fragment_pos;

out vec4 color;

uniform sampler2D tex_height;

uniform vec3 water_color;
uniform	float water_height;
uniform	float water_transparency;
uniform	float water_depth_alpha_factor;
uniform float water_depth_color_factor;
uniform float water_reflection_factor;

void main() {

	vec2 uv = vec2((fragment_pos[0] + 1)/2.0f, 1 - (fragment_pos[2] + 1)/2.0f);
    
	float grid_height = texture(tex_height, uv)[0];

	// Check if we need to take it into account
	if (fragment_pos[1] < grid_height) {
		// Solid base
		color = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	} else if (fragment_pos[1] < water_height) {
		// Water
		color = vec4(0.5, 0.5, 1, 0.5);//vec4(mix(texture_color, mirror_color, vec3(water_reflection_factor)), alpha);
	} else {
		// Air
		discard;
	}

	//texture_color = texture_color/(water_depth_color_factor * depth + 1);
	//float alpha = water_transparency + (1 - water_transparency) * (water_depth_alpha_factor * depth);

    // Mix the two together
    //color = vec4(0.5, 0.5, 1, 1);//vec4(mix(texture_color, mirror_color, vec3(water_reflection_factor)), alpha);
}