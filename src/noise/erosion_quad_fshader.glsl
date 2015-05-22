#version 330 core

in vec2 uv;

layout (location = 0) out vec4 tex_out_height;
layout (location = 1) out vec4 tex_out_dirt;
layout (location = 2) out vec4 tex_out_pos;

// Input textures
layout (location = 0) uniform sampler2D tex_in_height;
layout (location = 1) uniform sampler2D tex_in_dirt;
layout (location = 2) uniform sampler2D tex_in_pos;

uniform float dirt_max_slope;
uniform float dirt_max_height;
uniform float dirt_max_amount;
uniform float dirt_amount;
uniform float dirt_time;

uniform float DX;
uniform float DY;
uniform float DZ;

uniform int action;

const int ACTION_CREATE = 0;
const int ACTION_LOWER = 1;
const int ACTION_SOLIDIFY = 2;
const int ACTION_LEVEL = 3;

const float MAX_DIRT = 0.01;

uniform float dirt_threshold;

void main() {

	float height = texture(tex_in_height, uv)[0];
	if (height < dirt_max_height) {
		if (action == ACTION_CREATE) {
			float height = texture(tex_in_height, uv)[0];
			float diff = dirt_max_height - height;

			float dirt = min(diff * 0.5, dirt_amount);
			//height -= dirt;

			tex_out_height = vec4(height, 0, 0, 1);
			tex_out_dirt = vec4(dirt, 0, 0, 1);
		}

		if (action == ACTION_LOWER) {
		
			float height = texture(tex_in_height, uv)[0];
			float dirt = texture(tex_in_dirt, uv)[0];
			float level = height + dirt;

			float min_level = level;
			float min_i = 0;
			float min_j = 0;
			for(int i = -1; i <= 1; i++) {
				for(int j = -1; j <= 1; j++) {
					if (i != 0 || j != 0) { 
						vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);

						vec4 height_ij = texture(tex_in_height, uv_ij);
						vec4 dirt_ij = texture(tex_in_dirt, uv_ij);
						vec4 pos_ij = texture(tex_in_pos, uv_ij);

						float level_ij = height_ij[0] + dirt_ij[0];

						if (pos_ij[0] == -i && pos_ij[1] == -j) {
							dirt += dirt_ij[1];
						}

						if (level_ij < min_level) {
							min_i = i;
							min_j = j;
							min_level = level_ij;
						}
					}
				}
			}

			float next_dirt;
			if (min_i != 0 && min_j != 0){

				float slope = (level - min_level) / DZ;
				if (slope > dirt_max_slope) {
					float diff = (level - min_level) - DZ * dirt_max_slope;
					float extra = min(diff, dirt);
			
					dirt = dirt - extra;
					next_dirt = extra;
				}
			} else if (min_i != 0 || min_j != 0){

				float slope = (level - min_level) / DX;
				if (slope > dirt_max_slope) {
					float diff = (level - min_level) - DX * dirt_max_slope;
					float extra = min(diff, dirt);
					dirt -= extra;
					next_dirt = extra;
				}
			}

			dirt = min(dirt, dirt_max_amount);
			dirt = max(dirt, 0);
			tex_out_height = vec4(height, 0, 0, 1);
			tex_out_dirt = vec4(dirt, next_dirt, 0, 1);
			tex_out_pos = vec4(min_i, min_j, 0, 1);
		}

		else if (action == ACTION_SOLIDIFY) {

			float height = texture(tex_in_height, uv)[0];
			float dirt = texture(tex_in_dirt, uv)[0];
			
			for(int i = -1; i <= 1; i++) {
				for(int j = -1; j <= 1; j++) {
					if (i != 0 || j != 0) { 
						vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);

						vec4 dirt_ij = texture(tex_in_dirt, uv_ij);
						vec4 pos_ij = texture(tex_in_pos, uv_ij);

						if (pos_ij[0] == -i && pos_ij[1] == -j) {
							dirt += dirt_ij[1];
						}
					}
				}
			}

			dirt = min(dirt, dirt_max_amount);
			dirt = max(dirt, 0);
			height += dirt;
			tex_out_height = vec4(height, 0, 0, 1);
			tex_out_dirt = vec4(dirt, 0, 0, 1);
			tex_out_pos = vec4(0, 0, 0, 1);
		}

		else if (action == ACTION_LEVEL) {
			float height = texture(tex_in_height, uv)[0];
			float dirt = texture(tex_in_dirt, uv)[0];
			
			if (dirt > dirt_threshold) {
				float sum = height;
				for(int i = -1; i <= 1; i++) {
					for(int j = -1; j <= 1; j++) {
						if (i != 0 || j != 0) { 
							vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);
							float height_ij = texture(tex_in_height, uv_ij)[0];

							sum += height_ij;
						}
					}
				}

				height = sum / 9.0;
			} else {
				dirt = 0;
			}

			tex_out_height = vec4(height, 0, 0, 1);
			tex_out_dirt = vec4(dirt, 0, 0, 1);
		}
	} else {
		float height = texture(tex_in_height, uv)[0];
		float dirt = 0.0; 
		float next_dirt = 0.0;

		float min_i = 0.0;
		float min_j = 0.0;

		tex_out_height = vec4(height, 0, 0, 1);
		tex_out_dirt = vec4(dirt, next_dirt, 0, 1);
		tex_out_pos = vec4(min_i, min_j, 0, 1);
	}
}