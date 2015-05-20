#include "noise_generator.h"

NoiseGenerator::NoiseGenerator(GLuint* tex_height, NoiseParams* noise_params, ErosionParams* erosion_params) :
		_noise_params(noise_params),
		_erosion_params(erosion_params)
	{
		_tex_height = tex_height;
	}

void NoiseGenerator::init() {
		_framebuffer[0].resize(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
		_framebuffer[0].init_texture();
		_framebuffer[0].init(GL_R32F, GL_RED, GL_FLOAT);

		_framebuffer[1].resize(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
		_framebuffer[1].init_texture();
		_framebuffer[1].init(GL_R32F, GL_RED, GL_FLOAT);

		_erosionbuffer[0].resize(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
		_erosionbuffer[1].resize(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
		_erosionbuffer[0].init();
		_erosionbuffer[1].init();

		_copybuffer.resize(PIXELS_PER_UNIT, PIXELS_PER_UNIT);

		_quad.init();
		_copy_quad.init();
		_erosion_quad.init();
	}

void NoiseGenerator::renderNoise(int out, int in, NoiseParams* noise_params, float noise_amplitude) {
	///--- Render random noise on quad in the framebuffer
	_framebuffer[out].bind();
		glClear(GL_COLOR_BUFFER_BIT);
		_quad.drawNoise(noise_params, noise_amplitude, _framebuffer[in].get_tex());
	_framebuffer[out].unbind();
}


void NoiseGenerator::renderFractal() {
	_framebuffer[0].clear();
	_framebuffer[1].clear();
		
	NoiseParams noise_params_tmp = _noise_params->copy();
	noise_params_tmp.amplitude = 1;

	// Two variables to swap buffers
	int in = 0;
	int out = 1;

	if (_noise_params->fractal_type == MULTIFRACTAL) {
		renderNoise(in, out, &FLAT_NOISE, 1);
	}

	for (int i = 0; i < _noise_params->octaves; i++) {
		renderNoise(out, in, &noise_params_tmp, noise_params_tmp.amplitude);

		noise_params_tmp.height *= _noise_params->lacunarity;
		noise_params_tmp.width *= _noise_params->lacunarity;
		noise_params_tmp.amplitude *= pow(_noise_params->lacunarity, -_noise_params->H);

		// Swap input & output textures
		in = 1 - in;
		out = 1 - out;
	}

	copyNoise(_framebuffer[in].get_tex(), _tex_height, _noise_params->amplitude, _noise_params->offset);
}

void NoiseGenerator::erode() {
	/*GLuint* tex_height;
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
		
		_erosionbuffer[out].bind();
			glClear(GL_COLOR_BUFFER_BIT);
			_erosion_quad.draw(tex_height, tex_water, tex_sediment, tex_pos, _erosion_params);
		_erosionbuffer[out].unbind();

		in = 1 - in;
		out = 1 - out;
	}

	copyTexture(_erosionbuffer[in].get_tex_height(), _tex_height);*/
}


void NoiseGenerator::copyTexture(GLuint* src, GLuint* dst) {
	_copybuffer.set_texture(dst);
	_copybuffer.init(GL_R32F, GL_RED, GL_FLOAT);
	_copybuffer.bind();
		glClear(GL_COLOR_BUFFER_BIT);
		_copy_quad.drawTexture(src);
	_copybuffer.unbind();
}

void NoiseGenerator::copyNoise(GLuint* src, GLuint* dst, float amplitude, float offset) {
	///--- Render random noise on quad in the framebuffer
	_copybuffer.set_texture(dst);
	_copybuffer.init(GL_R32F, GL_RED, GL_FLOAT);
	_copybuffer.bind();
		glClear(GL_COLOR_BUFFER_BIT);
		_copy_quad.drawTexture(src, amplitude, offset);
	_copybuffer.unbind();
}
