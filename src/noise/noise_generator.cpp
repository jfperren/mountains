#include "noise_generator.h"

NoiseGenerator::NoiseGenerator(GLuint* tex_height, GLuint* tex_snow)
	{
		_tex_height = tex_height;
		_tex_snow = tex_snow;

		glGenTextures(1, _tex_height);
		glBindTexture(GL_TEXTURE_2D, *_tex_height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenTextures(1, _tex_snow);
		glBindTexture(GL_TEXTURE_2D, *_tex_snow);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

void NoiseGenerator::init(AppParams* app_params) {

	_noise_params = app_params->noise_params;
	_snow_params = app_params->snow_params;
	_erosion_params = app_params->erosion_params;

	_framebuffer[0].resize(_noise_params->resolution, _noise_params->resolution);
	_framebuffer[1].resize(_noise_params->resolution, _noise_params->resolution);
	_erosionbuffer[0].resize(_noise_params->resolution, _noise_params->resolution);
	_erosionbuffer[1].resize(_noise_params->resolution, _noise_params->resolution);

	_framebuffer[0].init();
	_framebuffer[1].init();
	_erosionbuffer[0].init();
	_erosionbuffer[1].init();

	_copybuffer.resize(_noise_params->resolution, _noise_params->resolution);

	_quad.init(app_params);
}

void NoiseGenerator::renderNoise(int out, int in, NoiseParams* noise_params, float noise_amplitude) {
	///--- Render random noise on quad in the framebuffer
	_framebuffer[out].bind();
		glClear(GL_COLOR_BUFFER_BIT);
		_quad.drawNoise(noise_params, noise_amplitude, _framebuffer[in].get_tex());
	_framebuffer[out].unbind();
}


void NoiseGenerator::renderFractal() {
	_framebuffer[0].resize(_noise_params->resolution, _noise_params->resolution);
	_framebuffer[1].resize(_noise_params->resolution, _noise_params->resolution);
	_framebuffer[0].init();
	_framebuffer[1].init();

	_quad.setShaders(NOISE_MODE);
	_quad.genVertexArray();

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

GLuint* NoiseGenerator::get_tex_height() {
	return _tex_height;
}

void NoiseGenerator::erode() {
/*	GLuint* tex_height;
	GLuint* tex_water;
	GLuint* tex_sediment;
	GLuint* tex_pos;

	_erosionbuffer[0].resize(_noise_params->resolution, _noise_params->resolution);
	_erosionbuffer[1].resize(_noise_params->resolution, _noise_params->resolution);
	_erosionbuffer[0].init();
	_erosionbuffer[1].init();
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

void NoiseGenerator::addSnow() {
	GLuint* tex_snow;
	GLuint* tex_height;
	GLuint* tex_pos;

	_erosionbuffer[0].resize(_noise_params->resolution, _noise_params->resolution);
	_erosionbuffer[1].resize(_noise_params->resolution, _noise_params->resolution);
	_erosionbuffer[0].init();
	_erosionbuffer[1].init();
	_erosionbuffer[0].clear();
	_erosionbuffer[1].clear();

	_quad.setShaders(SNOW_MODE);
	_quad.genVertexArray();

	if (_snow_params->enable) {
		int in = 0;
		int out = 1;

		tex_height = _tex_height;
		tex_snow = _erosionbuffer[in].get_tex_snow();
		tex_pos = _erosionbuffer[in].get_tex_pos();

		_erosionbuffer[out].bind();
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.fall(tex_height, tex_snow, tex_pos);
		_erosionbuffer[out].unbind();

		in = 1 - in;
		out = 1 - out;
		
		for (int t = 0; t < _snow_params->slide_time; t++) {

			tex_height = _erosionbuffer[in].get_tex_height();
			tex_snow = _erosionbuffer[in].get_tex_snow();
			tex_pos = _erosionbuffer[in].get_tex_pos();

			_erosionbuffer[out].bind();
				glClear(GL_COLOR_BUFFER_BIT);
				_quad.slide(tex_height, tex_snow, tex_pos);
			_erosionbuffer[out].unbind();

			in = 1 - in;
			out = 1 - out;
		}

		for (int t = 0; t < _snow_params->melt_time; t++) {
			tex_height = _erosionbuffer[in].get_tex_height();
			tex_snow = _erosionbuffer[in].get_tex_snow();
			tex_pos = _erosionbuffer[in].get_tex_pos();

			_erosionbuffer[out].bind();
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.melt(tex_height, tex_snow, tex_pos);
			_erosionbuffer[out].unbind();

			in = 1 - in;
			out = 1 - out;
		}
		
		for (int t = 0; t < _snow_params->smooth_time; t++) {
			tex_height = _erosionbuffer[in].get_tex_height();
			tex_snow = _erosionbuffer[in].get_tex_snow();
			tex_pos = _erosionbuffer[in].get_tex_pos();

			_erosionbuffer[out].bind();
				glClear(GL_COLOR_BUFFER_BIT);
				_quad.smooth(tex_height, tex_snow, tex_pos);
			_erosionbuffer[out].unbind();

			in = 1 - in;
			out = 1 - out;
		}

		tex_height = _erosionbuffer[in].get_tex_height();
		tex_snow = _erosionbuffer[in].get_tex_snow();

		copyTexture(tex_height, _tex_height);
		copyTexture(tex_snow, _tex_snow);
	}
}

void NoiseGenerator::copyTexture(GLuint* src, GLuint* dst) {
	_copybuffer.resize(_noise_params->resolution, _noise_params->resolution);
	_copybuffer.init(dst);

	_quad.setShaders(COPY_MODE);
	_quad.genVertexArray();
	cout << "COPY WITH RESOLUTION " << _noise_params->resolution << endl;
	_copybuffer.bind();
		glClear(GL_COLOR_BUFFER_BIT);
		 _quad.drawTexture(src);
	_copybuffer.unbind();
}

void NoiseGenerator::copyNoise(GLuint* src, GLuint* dst, float amplitude, float offset) {
	///--- Render random noise on quad in the framebuffer
	_copybuffer.resize(_noise_params->resolution, _noise_params->resolution);
	_copybuffer.init(dst);
	_copybuffer.clear();

	_quad.setShaders(COPY_MODE);
	_quad.genVertexArray();

	_copybuffer.bind();
		glClear(GL_COLOR_BUFFER_BIT);
		_quad.drawTexture(src, amplitude, offset);
	_copybuffer.unbind();
}
