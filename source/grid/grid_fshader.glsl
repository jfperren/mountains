#version 330 core

uniform sampler2D tex_height;

uniform sampler2D tex_grass;
uniform sampler2D tex_snow;
uniform sampler2D tex_rock;
uniform sampler2D tex_sand;
uniform sampler2D tex_rock_underwater;

uniform vec3 light_pos;
uniform vec3 Ia, Id;

const float pixel_unit = 1.0/2048.0;

uniform int only_reflect;

out vec4 color;

in float height;

in vec2 uv;

/** The higher point reached by water */
const float see_level = 0.0;

/** The higher point of the beach */
const float sand_offset = 0.05;

const float vegetation_start = 0.02;
const float vegetation_threshold = 0.25;

/** The point where snow begins to appear */
const float snow_threshold = 0.6;

const float snow_offset = 0.02;

/* range from 0 to 1 in the reals.
   0: more grass
   1: more rock*/
const float scale_factor_rock_grass = 0.3;

/* range from 0 to 1 in the reals.
   0: more rock
   1: more sand*/
const float scale_factor_rock_sand = 0.6;

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

float clamp_height(float height) {
	float height_to_texture = height; 
	if (height >= 0.99){
		height_to_texture = 0.99;
	}
	else if (height <= -0.99) {
		height_to_texture = -0.99;
	}

	return height_to_texture;
}

void main() {

	float height_to_texture = clamp_height(height);
	vec3 derivative_x = vec3(1, 0, 1000*texture(tex_height, uv + vec2(pixel_unit,0))[0] - 1000*texture(tex_height, uv - vec2(pixel_unit,0))[0]);
	vec3 derivative_y = vec3(0, 1, 1000*texture(tex_height, uv + vec2(0, pixel_unit))[0] - 1000*texture(tex_height, uv - vec2(0, pixel_unit))[0]);
	vec3 normal = normalize(cross(derivative_x, derivative_y));



	float alpha = 1 - dot(normal, vec3(0, 0, 1));

	// Default (for debug) texture is gray
	vec3 texture_tot = vec3(0.5, 0.5, 0.5);

	if (height_to_texture <= see_level) {
		// Mix sand and rock
		texture_tot = mix(texture(tex_rock, 60 * uv).xyz, texture(tex_rock_underwater, 10 * uv).xyz, alpha * scale_factor_rock_sand);
	} else if (height_to_texture <= vegetation_start - 0.005) {
		// Mix sand + rock (depending on the steepness)
		texture_tot = mix(texture(tex_rock, 60 * uv).xyz, texture(tex_rock_underwater, 10 * uv).xyz, alpha * 0.1);

	} else if (height_to_texture <= vegetation_start + 0.005) {
		// Fade from sand to grass
		vec3 temp = mix(texture(tex_rock, 60 * uv).xyz, texture(tex_rock_underwater, 10 * uv).xyz, alpha * 0.1);
		vec3 rock_grass = mix(texture(tex_grass, 10 * uv).xyz, texture(tex_snow, 10 * uv).xyz, alpha * scale_factor_rock_grass);
		texture_tot = mix(temp, rock_grass, compute_linear_interpolation(vegetation_start - 0.005, vegetation_start + 0.005, height_to_texture));

	} else if (height_to_texture <= snow_threshold - snow_offset) {
		texture_tot = mix(texture(tex_grass, 10 * uv).xyz, texture(tex_snow, 10 * uv).xyz, alpha * scale_factor_rock_grass);
	}
	else if (height_to_texture <= snow_threshold + snow_offset) {
		// height > snow_threshold
		// Mix grass+rock and snow

		vec3 temp = mix(texture(tex_grass, 10 * uv).xyz, texture(tex_snow, 10 * uv).xyz, alpha * scale_factor_rock_grass);
		texture_tot = mix(temp, texture(tex_sand, 30 * uv).xyz, compute_linear_interpolation(snow_threshold - snow_offset, snow_threshold + snow_offset, height_to_texture));


	} else {
		texture_tot = texture(tex_sand, 30 * uv).xyz;
	}

	//texture_tot = mix(texture(grass, 10 * uv).xyz, texture(rock, 10 * uv).xyz, alpha*scale_factor);

   	vec3 ambient = Ia * texture_tot;
  	vec3 diffuse = Id * dot(normal, normalize(light_pos));


	if (only_reflect == 1 && height < 0) {
		discard;
	} else {
		color = vec4(ambient + diffuse, 1.0f);
	}
}
