#pragma once

#include "icg_common.h"
#include "../app/constants.h"

class CopyQuad {

private:
	GLuint _vao; ///< vertex array object
	GLuint _pid; ///< GLSL shader program ID 
	GLuint _vbo; ///< memory buffer

public:
	void init();
	void cleanup();
	void drawTexture(GLuint* texture);
};
