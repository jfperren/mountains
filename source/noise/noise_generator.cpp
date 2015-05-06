#include "noise_generator.h"

#pragma once

#include "icg_common.h"
#include "../framebuffer/framebuffer.h"
#include "noise_quad.h"
#include "../constants.h"

NoiseGenerator::NoiseGenerator(GLuint* out_tex, NoiseParams* noise_params) :
		_noise_params(noise_params)
	{
		_tex[0] = (GLuint*)malloc(sizeof(GLuint));
		_tex[1] = (GLuint*)malloc(sizeof(GLuint));
		_tex[2] = (GLuint*)malloc(sizeof(GLuint));

		_tex[0] = out_tex;
	}

void NoiseGenerator::init() {
		_framebuffer[0].resize(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
		_framebuffer[0].set_texture(_tex[0]);
		_framebuffer[0].init(GL_R32F, GL_RED, GL_FLOAT);

		_framebuffer[1].resize(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
		*_tex[1] = _framebuffer[1].init_texture();
		_framebuffer[1].init(GL_R32F, GL_RED, GL_FLOAT);

		_framebuffer[2].resize(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
		*_tex[2] = _framebuffer[2].init_texture();
		_framebuffer[2].init(GL_R32F, GL_RED, GL_FLOAT);

		_quad.init();
	}

void NoiseGenerator::renderNoise(int out, int in, NoiseParams* noise_params, float noise_amplitude) {
	///--- Render random noise on quad in the framebuffer
	_framebuffer[out].bind();
		glClear(GL_COLOR_BUFFER_BIT);
		_quad.drawNoise(noise_params, noise_amplitude, _tex[in]);
	_framebuffer[out].unbind();
}

void NoiseGenerator::copyNoise(int out, int in, NoiseParams* noise_params) {
	///--- Render random noise on quad in the framebuffer
	_framebuffer[out].bind();
		glClear(GL_COLOR_BUFFER_BIT);
		_quad.copyNoise(noise_params, _tex[in]);
	_framebuffer[out].unbind();
}

void NoiseGenerator::renderFractal() {
	_framebuffer[0].clear();
	_framebuffer[1].clear();
	_framebuffer[2].clear();
		
	NoiseParams noise_params_tmp = _noise_params->copy();

	// Two variables to swap buffers
	int in = 1;
	int out = 2;

	if (_noise_params->fractal_type == MULTIFRACTAL) {
		renderNoise(in, out, &FLAT_NOISE, 1);
	}

	for (int i = 0; i < _noise_params->octaves; i++) {
		renderNoise(out, in, &noise_params_tmp, noise_params_tmp.amplitude);

		noise_params_tmp.height *= _noise_params->lacunarity;
		noise_params_tmp.width *= _noise_params->lacunarity;
		noise_params_tmp.amplitude *= pow(_noise_params->lacunarity, -_noise_params->H);

		// Swap input & output textures
		in = (in == 1 ? 2 : 1);
		out = (out == 1 ? 2 : 1);
	}

	// Render created texture in out_texture with offset and amplitude
	copyNoise(0, in, _noise_params);
}
