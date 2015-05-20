#include "noise_generator.h"

NoiseGenerator::NoiseGenerator(GLuint* out_tex, NoiseParams* noise_params, ErosionParams* erosion_params) :
		_noise_params(noise_params),
		_erosion_params(erosion_params)
	{
		_tex[0] = (GLuint*)malloc(sizeof(GLuint));
		_tex[1] = (GLuint*)malloc(sizeof(GLuint));
		_tex[2] = (GLuint*)malloc(sizeof(GLuint));

		_tex[0] = out_tex;

		_tex_height = out_tex;
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

		_erosionbuffer[0].resize(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
		_erosionbuffer[1].resize(PIXELS_PER_UNIT, PIXELS_PER_UNIT);

		_erosionbuffer[0].init();
		_erosionbuffer[1].init();

		_quad.init();
		_erosion_quad.init();
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
	noise_params_tmp.amplitude = 1;

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

void NoiseGenerator::erode() {
	GLuint* tex_height;
	GLuint* tex_water;
	GLuint* tex_sediment;
	GLuint* tex_pos;

	_erosionbuffer[0].clear();
	_erosionbuffer[1].clear();

	int in = 0;
	int out = 1;

	for (int t = 0; t < _erosion_params->iterations; t++) {
		tex_height		= (t == 0) ? _tex_height : _erosionbuffer[in].get_tex_height();
		tex_water		= _erosionbuffer[in].get_tex_water();
		tex_sediment	= _erosionbuffer[in].get_tex_sediment();
		tex_pos			= _erosionbuffer[in].get_tex_pos();

		_framebuffer[out].bind();
			glClear(GL_COLOR_BUFFER_BIT);
			_erosion_quad.draw(tex_height, tex_water, tex_sediment, tex_pos, _erosion_params);
		_framebuffer[out].unbind();

		in = 1 - in;
		out = 1 - out;
	}

	_tex_height = _erosionbuffer[in].get_tex_height();
}
