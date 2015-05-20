#pragma once

#include "icg_common.h"
#include "../app/constants.h"

class ErosionQuad {

private:
	GLuint _vao; ///< vertex array object
	GLuint _pid; ///< GLSL shader program ID 
	GLuint _vbo; ///< memory buffer
	GLuint _tex; ///< Texture ID

public:
	void init();
	void cleanup();
	void drawErosion(GLuint* tex_noise, ErosionParams* erosion_params);
};
