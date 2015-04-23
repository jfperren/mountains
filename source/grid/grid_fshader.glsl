#version 330 core

uniform sampler2D tex_height;

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
const float sand_offset = 0.005;

const float vegetation_threshold = 0.2;

/** The point where snow begins to appear */
const float snow_threshold = 0.7;

/* range from 0 to 1 in the reals.
   1: more rock
   2: more grass*/
const float scale_factor = 0.8;


void main() {

	float height_to_texture;
	vec3 derivative_x = vec3(1, 0, 1000*texture(tex_height, uv + vec2(pixel_unit,0))[0] - 1000*texture(tex_height, uv - vec2(pixel_unit,0))[0]);
	vec3 derivative_y = vec3(0, 1, 1000*texture(tex_height, uv + vec2(0, pixel_unit))[0] - 1000*texture(tex_height, uv - vec2(0, pixel_unit))[0]);
	vec3 normal = normalize(cross(derivative_x, derivative_y));

	if (height >= 0.99){
		height_to_texture = 0.99;
	}
	else if (height <= -0.99) {
		height_to_texture = -0.99;
	}
	else {
		height_to_texture = height; 
	}

	float alpha = 1 - dot(normal, vec3(0, 0, 1));

	vec3 texture_tot = vec3(0.5, 0.5, 0.5);

	if (height_to_texture <= see_level + sand_offset) {
		// Mix sand and rock
		texture_tot = mix(texture(tex_texture3, 60 * uv).xyz, texture(tex_texture2, 10 * uv).xyz, alpha * 0.7);
	} else if (height_to_texture <= vegetation_threshold) {
		// Mix grass and rock
		texture_tot = mix(texture(tex_texture1, 10 * uv).xyz, texture(tex_texture2, 10 * uv).xyz, alpha * scale_factor);
		//texture_tot = vec3(0.5, 0.5, 0.5);
	} else {
		// height > snow_threshold
		// Mix grass+rock and snow
		float beta = (0 - 1) / (vegetation_threshold - snow_threshold) * height_to_texture + (vegetation_threshold * 1 - snow_threshold * 0) / (vegetation_threshold - snow_threshold);
		if (beta > 0.99) {
			beta = 1;
		}

		vec3 temp = mix(texture(tex_texture1, 10 * uv).xyz, texture(tex_texture2, 10 * uv).xyz, alpha * scale_factor);
		texture_tot = mix(temp, texture(tex_texture4, 30 * uv).xyz, beta);
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
