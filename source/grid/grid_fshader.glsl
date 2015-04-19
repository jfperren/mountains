#version 330 core

uniform sampler2D heightmap;
uniform sampler2D color1D;
uniform vec3 light_pos;
uniform vec3 Ia, Id;

const float pixel_unit = 1.0/2048.0;

uniform int only_reflect;

out vec3 color;

in float height;

in vec2 uv;


void main() {

	float height_to_texture;
	vec3 derivative_x = vec3(1, 0, 1000*texture(heightmap, uv + vec2(pixel_unit,0))[0] - 1000*texture(heightmap, uv - vec2(pixel_unit,0))[0]);
	vec3 derivative_y = vec3(0, 1, 1000*texture(heightmap, uv + vec2(0, pixel_unit))[0] - 1000*texture(heightmap, uv - vec2(0, pixel_unit))[0]);
	vec3 normal = normalize(cross(derivative_x, derivative_y));
	if(height >= 0.99){
		height_to_texture = 0.99;
	}
	else if(height <= -0.99) {
		height_to_texture = -0.99;
	}
	else{
		height_to_texture = height;
	}
   	vec3 ambient = Ia * texture(color1D, vec2((height_to_texture + 1)/2.0, 0.0)).rgb;
  	vec3 diffuse = Id * dot(normal, normalize(light_pos));

	if (only_reflect == 1 && texture(heightmap, uv)[0] < 0) {
		color = vec3(1.0, 1.0, 1.0);
	} else {
		color = ambient + diffuse;
	}
}
