#pragma once

#include "icg_common.h"
#include "../buffers/noisebuffer.h"
#include "../buffers/copybuffer.h"
#include "../buffers/erosionbuffer.h"
#include "../buffers/generalbuffer.h"
#include "quad.h"
#include "../app/constants.h"


class Terrain {

private:
	Generalbuffer _snowbuffer[2];
	Generalbuffer _noisebuffer[2];

	Copybuffer _copybuffer;

	GLuint _tex_height;
	GLuint _tex_snow;
	GLuint _tex_dirt;

	Quad _quad;

	NoiseParams* _noise_params;
	SnowParams* _snow_params;
	ErosionParams* _erosion_params;

public:
	Terrain();

	void init(AppParams* app_params);

	void copyTexture(GLuint* src, GLuint* dst);
	void copyNoise(GLuint* src, GLuint* dst, float amplitude, float offset);

	void renderFractal();
	void erode();
	void addSnow();

	GLuint* getHeightTexture();
	GLuint* getDirtTexture();
	GLuint* getSnowTexture();

	void resize();
};
