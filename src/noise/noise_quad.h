#pragma once

#include "icg_common.h"
#include "../app/constants.h"

class NoiseQuad {

private:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID 
    GLuint _vbo; ///< memory buffer
    GLuint _tex; ///< Texture ID

public:
	void init();
	void cleanup();
	void drawNoise(NoiseParams* noise_params, float noise_amplitude, GLuint* in_texture = nullptr);
	void copyNoise(NoiseParams* noise_params, GLuint* in_texture);
};
