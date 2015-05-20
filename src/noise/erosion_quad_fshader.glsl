#version 330 core

in vec2 uv;

layout (location = 0) out vec3 tex_out_height;
layout (location = 1) out vec3 tex_out_sediment;
layout (location = 2) out vec3 tex_out_water;
layout (location = 3) out vec3 tex_out_pos;

// Input textures
uniform sampler2D tex_in_height;
uniform sampler2D tex_in_water;
uniform sampler2D tex_in_sediment;
uniform sampler2D tex_in_pos;

uniform float erosion_deposition_rate;
uniform float erosion_evaporation_rate;
uniform float erosion_erosion_rate;
uniform float erosion_rain_rate;

uniform float erosion_sediment_capacity;
uniform float erosion_sediment_inertia;

const float DX = 1/2048.0;
const float DY = 1/2048.0;

void main() {
	// Recuperate values from previous iteration in tex_erosion
	float height = texture(tex_in_height, uv)[0];
	float water = texture(tex_in_water, uv)[0];
	float sediment = texture(tex_in_sediment, uv)[0];

	// 0. Get water/sediment from neighbours

	for(int i = -1; i <= 1; i++) {
		for(int j = -1; j <= 1; j++) {
			vec2 uv_neighbour = uv + vec2(i * DX, j * DY);
			vec4 pos_neighbour = texture(tex_in_pos, uv_neighbour);

			if (pos_neighbour[0] == -i && pos_neighbour[1] == -j){
				water += texture(tex_in_water, uv_neighbour)[1];
				sediment += texture(tex_in_sediment, uv_neighbour)[1];
			}
		}
	}

	// Compute gradient
	//float dx = texture(uv + vec2(DX, 0), tex_noise)[0] -texture(uv - vec2(DX, 0), tex_height)[0];
	//float dy = texture(uv + vec2(0, DY), tex_noise)[0] -texture(uv - vec2(0, DY), tex_height)[0];
	//vec2 gradient = vec2(dx, dy);

	// 1. Rainfall
	water += erosion_rain_rate;

	// 2. Erosion
	sediment += erosion_erosion_rate;

	// 3. Movement

	float level = height + water;

	// 3.1 Find lowest neighbour
	float lowest_level = level;
	float lowest_i = 0;
	float lowest_j = 0;

	for(int i = -1; i <= 1; i++) {
		for(int j = -1; j <= 1; j++) {
			vec2 uv_neighbour = uv + vec2(i * DX, j * DY);

			float neighbour_height = texture(tex_in_height, uv_neighbour)[0];
			float neighbour_water = texture(tex_in_water, uv_neighbour)[0];

			float neighbour_level = neighbour_height + neighbour_water;

			if (neighbour_level < lowest_level) {
				lowest_level = neighbour_level;
				lowest_i = i;
				lowest_j = j;
			}
		}
	}
	
	// 3.2 Move water

	float sediment_ratio = sediment / water;
	float next_sediment;
	float next_water;

	if (lowest_level != level) {
		float water_diff;
		float sediment_diff;

		if (lowest_level < height) {
			water_diff = water;
		} else {
			water_diff = (lowest_level - level)/2.0;
		}

		// Transfer everything
		next_water = water_diff;
		water -= water_diff;

		sediment_diff += sediment_ratio * water_diff * (1 - erosion_sediment_inertia);
		next_sediment = sediment_diff;
		sediment -= sediment_diff;
	}

	// 4. Evaporation

	water -= erosion_evaporation_rate;
	float max_sediment = erosion_sediment_capacity * water;

	if (sediment > max_sediment) {
		float extra_sediment = sediment - max_sediment;
		height += extra_sediment;
		sediment = max_sediment;
	}

	// 5. Write results

	gl_FragData[0] = vec4(height, 0 , 0, 0);
	gl_FragData[2] = vec4(sediment, next_sediment, 0, 0);
	gl_FragData[1] = vec4(water, next_water, 0, 0);
	gl_FragData[3] = vec4(lowest_i, lowest_j, 0, 0);
}