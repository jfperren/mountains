#pragma once

#include "icg_common.h"
#include "../app/constants.h"

using namespace std;

class Grid {

	string TEX_PATH = "textures/terrains/mountain/";
	string TEX_EXT = ".tga";

	const int GRASS = 0;
	const int SAND = 1;
	const int ROCK = 2;
	const int SNOW = 3;
	const int ROCK_UNDERWATER = 4;

	const int TEXTURES_COUNT = 5;

	const char* TEX_PATHS[5];
	string TEX_NAMES[5];

private:
	static const int grid_dim_ = 2048;

	LightParams* _light_params;
	GridParams* _grid_params;
	TextureParams* _texture_params;

	mat4 model = mat4::Identity();

protected:
    GLuint _vao;          ///< vertex array object
    GLuint _vbo_position; ///< memory buffer for positions
    GLuint _vbo_index;    ///< memory buffer for indice
	GLuint _vbo;
    GLuint _pid;          ///< GLSL shader program ID
    GLuint _tex_height;    ///< HeightMap Texture ID

	GLuint _tex_main;

	vector<GLuint> _texs;     

    GLuint _num_indices;  ///< number of vertices to render
    
public:    
	void init(AppParams* app_params);
	void draw(const mat4& view, const mat4& projection, bool only_reflect = false);
	void cleanup();

	void set_height_texture(GLuint tex_height);
	int get_vertex_index(int i, int j);
};
