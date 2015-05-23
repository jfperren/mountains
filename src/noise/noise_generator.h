#pragma once

#include "icg_common.h"
#include "../buffers/noisebuffer.h"
#include "../buffers/copybuffer.h"
#include "../buffers/erosionbuffer.h"
#include "../buffers/generalbuffer.h"
#include "quad.h"
#include "../app/constants.h"


class NoiseGenerator {

private:
	Noisebuffer _framebuffer[2];
	Erosionbuffer _erosionbuffer[2];

	Generalbuffer _generalbuffer[2];

	Copybuffer _copybuffer;

	GLuint* _tex_height;
	GLuint* _tex_snow;

	Quad _quad;

	NoiseParams* _noise_params;
	SnowParams* _snow_params;
	ErosionParams* _erosion_params;

public:
	NoiseGenerator(GLuint* tex_height, GLuint* tex_snow);

	void init(AppParams* app_params);
	void copyTexture(GLuint* src, GLuint* dst);
	void renderNoise(int out, int in, NoiseParams* noise_params, float noise_amplitude);
	void copyNoise(GLuint* src, GLuint* dst, float amplitude, float offset);
	void renderFractal();
	void erode();
	void addSnow();
	GLuint* get_tex_height();

	void resize();
};
