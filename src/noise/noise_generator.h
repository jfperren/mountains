#pragma once

#include "icg_common.h"
#include "../buffers/framebuffer.h"
#include "../buffers/erosionbuffer.h"
#include "noise_quad.h"
#include "erosion_quad.h"
#include "../app/constants.h"


class NoiseGenerator {

private:
	// 0 = out, 1/2 = tmp
	Framebuffer _framebuffer[3];
	Erosionbuffer _erosionbuffer[3];

	GLuint* _tex[3];
	GLuint* _tex_erosion[3];

	GLuint* _tex_height;

	NoiseQuad _quad;
	ErosionQuad _erosion_quad;

	NoiseParams* _noise_params;
	ErosionParams* _erosion_params;

public:
	NoiseGenerator(GLuint* out_tex, NoiseParams* noise_params, ErosionParams* erosion_params);

	void init();

	void renderNoise(int out, int in, NoiseParams* noise_params, float noise_amplitude);
	void copyNoise(int out, int in, NoiseParams* noise_params);
	void renderFractal();
	void erode();
};
