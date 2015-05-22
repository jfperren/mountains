#pragma once

#include "icg_common.h"
#include "../app/constants.h"

class ErosionQuad {

private:
	GLuint _vao; ///< vertex array object
	GLuint _pid; ///< GLSL shader program ID 
	GLuint _vbo; ///< memory buffer
	GLuint _tex; ///< Texture ID

	DirtParams* _dirt_params;

public:
	void init(AppParams* app_params);
	void cleanup();
	void draw(GLuint* tex_noise, GLuint* tex_water, GLuint* tex_sediment, GLuint* tex_pos);

	void createDirt(GLuint* tex_noise, GLuint* tex_dirt, GLuint* tex_pos);
	void lowerDirt(GLuint* tex_noise, GLuint* tex_dirt, GLuint* tex_pos);
	void solidifyDirt(GLuint* tex_noise, GLuint* tex_dirt, GLuint* tex_pos);
	void levelDirt(GLuint* tex_noise, GLuint* tex_dirt, GLuint* tex_pos);
};
