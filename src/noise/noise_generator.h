#pragma once

#include "icg_common.h"
#include "../buffers/noisebuffer.h"
#include "../buffers/copybuffer.h"
#include "../buffers/erosionbuffer.h"
#include "noise_quad.h"
#include "erosion_quad.h"
#include "copy_quad.h"
#include "../app/constants.h"


class NoiseGenerator {

private:
	Noisebuffer _framebuffer[2];
	Erosionbuffer _erosionbuffer[2];
	Copybuffer _copybuffer;

	GLuint* _tex_height;

	CopyQuad _copy_quad;
	NoiseQuad _quad;
	ErosionQuad _erosion_quad;

	NoiseParams* _noise_params;
	ErosionParams* _erosion_params;

public:
	NoiseGenerator(GLuint* out_tex, NoiseParams* noise_params, ErosionParams* erosion_params);

	void init();
	void copyTexture(GLuint* src, GLuint* dst);
	void renderNoise(int out, int in, NoiseParams* noise_params, float noise_amplitude);
	void copyNoise(GLuint* src, GLuint* dst, float amplitude, float offset);
	void renderFractal();
	void erode();

	void resize();
};
