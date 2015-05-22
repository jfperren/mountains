#pragma once

#include "icg_common.h"
#include "../app/constants.h"

class ErosionQuad {

private:
	GLuint _vao; ///< vertex array object
	GLuint _pid; ///< GLSL shader program ID 
	GLuint _vbo; ///< memory buffer
	GLuint _tex; ///< Texture ID

	SnowParams* _snow_params;

public:
	void init(AppParams* app_params);
	void cleanup();

	void fall(GLuint* tex_noise, GLuint* tex_snow, GLuint* tex_pos);
	void slide(GLuint* tex_noise, GLuint* tex_snow, GLuint* tex_pos);
	void melt(GLuint* tex_noise, GLuint* tex_snow, GLuint* tex_pos);
	void smooth(GLuint* tex_noise, GLuint* tex_snow, GLuint* tex_pos);

	void draw(GLuint* tex_noise, GLuint* tex_snow, GLuint* tex_pos, int mode);
};
