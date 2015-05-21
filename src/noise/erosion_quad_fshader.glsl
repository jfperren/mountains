#version 330 core

in vec2 uv;

layout (location = 0) out vec4 tex_out_height;
layout (location = 1) out vec4 tex_out_water;
layout (location = 2) out vec4 tex_out_sediment;
layout (location = 3) out vec4 tex_out_pos;

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

uniform float DX;
uniform float DY;
uniform float DZ;

void main() {


	float height = texture(tex_in_height, uv)[0];
	float next_height;
	
	int min_i = 0;
	int min_j = 0;
	float min_height = height;
	
	for(int i = -1; i <= 1; i++) {
		for(int j = -1; j <= 1; j++) {
			if (i != 0 || j != 0) { 
				vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);
				vec4 height_ij = texture(tex_in_height, uv_ij);
				vec4 pos_ij = texture(tex_in_pos, uv_ij);

				if (pos_ij[0] == -i && pos_ij[1] == -j) {
					height += height_ij[1];
				}
				
				if (height > height_ij[0]) {
					min_i = i;
					min_j = j;
					min_height = height_ij[0];
				}
			}
		}
	}

	//height += texture(tex_in_water, uv)[0];
	
	if (min_i != 0 && min_j != 0){
		if ((height - min_height) / DZ > 1) {
			float diff = (height - min_height);
			float extra = (diff - DZ)/2;
			
			height -= extra * extra/diff;
			next_height = extra * extra/diff;
		}
	} else if (min_i != 0 || min_j != 0){
		if ((height - min_height) > DX) {
			float extra = (height - min_height) - DX;
			height -= DX;
			next_height = DX;
		}
	}

	tex_out_height  = vec4(height, next_height, 0, 1);
	tex_out_pos		= vec4(min_i, min_j, 0, 1);
	tex_out_water   = vec4(1, 0, 0, 1);
	
/*
	float height = texture(tex_in_height, uv)[0];
	float height_t = texture(tex_in_height, uv + vec2(0, DY))[0];
	float height_b = texture(tex_in_height, uv - vec2(0, DY))[0];
	float height_r = texture(tex_in_height, uv + vec2(DX, 0))[0];
	float height_l = texture(tex_in_height, uv - vec2(DX, 0))[0];

	if (height_t < height && (height - height_t)/DX > 1) {
		height = height_t + DX;
	}

	if (height_r < height && (height - height_r)/DX > 1) {
		height = height_r + DX;
	} 

	float height = texture(tex_in_height, uv)[0];
	float height_tr = texture(tex_in_height, uv + vec2(+DX, +DY))[0];
	float height_br = texture(tex_in_height, uv + vec2(+DX, -DY))[0];
	float height_tl = texture(tex_in_height, uv + vec2(-DX, +DY))[0];
	float height_bl = texture(tex_in_height, uv + vec2(-DX, -DY))[0];

	if (height_tr < height && (height - height_tr)/DZ > 1) {
		height = height_tr + DZ;
	}

	if (height_br < height && (height - height_br)/DZ > 1) {
		height = height_br + DZ;
	} 

	if (height_bl < height && (height - height_bl)/DZ > 1) {
		height = height_bl + DZ;
	} 
	if (height_tr < height && (height - height_tl)/DZ > 1) {
		height = height_tl + DZ;
	} 
*/

#if 0
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
				water += 1.0;
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
	sediment += water * erosion_erosion_rate;

	// 3. Movement

	float level = height + water;

	// 3.1 Find lowest neighbour

	float height_t = texture(tex_in_height, uv + vec2(0, DY)[0];
	float height_b = texture(tex_in_height, uv - vec2(0, DY)[0];
	float height_r = texture(tex_in_height, uv + vec2(DX, 0)[0];
	float height_l = texture(tex_in_height, uv - vec2(DX, 0)[0];

	vec4 water_t = texture(tex_in_water, uv + vec2(0, DY);
	vec4 water_b = texture(tex_in_water, uv - vec2(0, DY);
	vec4 water_r = texture(tex_in_water, uv + vec2(DX, 0);
	vec4 water_l = texture(tex_in_water, uv - vec2(DX, 0);

	float level_t = height_t + water_t;
	float level_b = height_b + water_b;
	float level_r = height_r + water_r;
	float level_l = height_l + water_l;

	for(int i = -1; i <= 1; i++) {
		for(int j = -1; j <= 1; j++) {
			if (i != 0 || j != 0) {
				vec4 height_ij = texture(tex_in_height, uv + vec2(i, j) * vec2(DX, DY));

				if (height_ij[2] == -i && height_ij[3] == -j) {
					height += height_ij[1];
				}

				if (height_ij[0] < min_height) {
					min_i = i;
					min_j = j;
					min_height = height_ij[0];
				}
			}
		}
	}
	
	float diff = 0;

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

	tex_out_height		= vec4(height, 0, 0, 1);
	tex_out_water		= vec4(water, next_water, 0, 1);
	tex_out_sediment	= vec4(water, next_sediment, 0, 1);
	tex_out_pos			= vec4(lowest_i, lowest_j, 0, 1);*/
	#endif
}