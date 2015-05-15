#pragma once

#include "icg_common.h"
#include "../app/constants.h"

class Water {

private:
	int grid_size = 2048;
	mat4 model = mat4::Identity();
	GridParams* _grid_params;
	WaterParams* _water_params;
	LightParams* _light_params;

	GLuint _vao;          ///< vertex array object
	GLuint _vbo;
	GLuint _tex_main;
	GLuint _tex_mirror;
	GLuint _tex_height;
	GLuint _tex_depth;
	GLuint _tex_normal_map;
	GLuint _pid;          ///< GLSL shader program ID
	GLuint _num_indices;  ///< number of vertices to render

public:
	void init(GridParams* grid_params, WaterParams* water_params, LightParams* light_params);
	void cleanup();
	void set_main_texture(GLuint tex_main);
	void set_mirror_texture(GLuint tex_mirror);
	void set_height_texture(GLuint tex_height);
	void set_depth_texture(GLuint tex_depth);
	void set_normal_map(GLuint tex_normal_map);
	void draw(const mat4& view, const mat4& projection);
};