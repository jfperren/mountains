#version 330 core

uniform sampler2D tex_height;

uniform sampler2D tex_texture0;
uniform sampler2D tex_texture1;
uniform sampler2D tex_texture2;
uniform sampler2D tex_texture3;
uniform sampler2D tex_texture4;

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
const float vegetation_threshold = 0.2;

/** The point where snow begins to appear */
const float snow_threshold = 0.5;

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
		texture_tot = mix(texture(tex_texture3, 60 * uv).xyz, texture(tex_texture0, 10 * uv).xyz, alpha * scale_factor_rock_sand);
	} else if (height_to_texture <= vegetation_start) {
		// Mix sand_rock and rock_grass

		vec3 sand_rock = mix(texture(tex_texture3, 60 * uv).xyz, texture(tex_texture0, 10 * uv).xyz, alpha * scale_factor_rock_sand);
		vec3 rock_grass = mix(texture(tex_texture1, 10 * uv).xyz, texture(tex_texture2, 10 * uv).xyz, alpha * scale_factor_rock_grass);

		texture_tot = mix(sand_rock, rock_grass, compute_linear_interpolation(see_level, vegetation_start, height_to_texture));
	} else {
		// height > snow_threshold
		// Mix grass+rock and snow

		vec3 temp = mix(texture(tex_texture1, 10 * uv).xyz, texture(tex_texture2, 10 * uv).xyz, alpha * scale_factor_rock_grass);
		texture_tot = mix(temp, texture(tex_texture4, 30 * uv).xyz, compute_linear_interpolation(vegetation_threshold, snow_threshold, height_to_texture));
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
