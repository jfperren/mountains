#include "noise_generator.h"

#define INIT(in, out) int in = 0; int out = 1;
#define SWAP(in, out) in = 1 - in; out = 1 - out;
#define SWAP_TEXTURE(tex, buffer, index) tex = buffer[in].getTexture(index);
#define CLEAR_BUFFERS(buffer) buffer[0].clear(); buffer[1].clear();
#define SET_MODE(mode) _quad.setShaders(mode);_quad.genVertexArray();

NoiseGenerator::NoiseGenerator(GLuint* tex_height, GLuint* tex_snow)
	{
		_tex_height = tex_height;
		_tex_snow = tex_snow;
	}

void NoiseGenerator::init(AppParams* app_params) {

	_noise_params = app_params->noise_params;
	_snow_params = app_params->snow_params;
	_erosion_params = app_params->erosion_params;

	_framebuffer[0].resize(_noise_params->resolution, _noise_params->resolution);
	_framebuffer[1].resize(_noise_params->resolution, _noise_params->resolution);
	_erosionbuffer[0].resize(_noise_params->resolution, _noise_params->resolution);
	_erosionbuffer[1].resize(_noise_params->resolution, _noise_params->resolution);

	_snowbuffer[0].init(3);
	_snowbuffer[0].genTextures();
	_snowbuffer[0].setFormat(GL_RG32F, GL_RED, GL_FLOAT);

	_snowbuffer[1].init(3);
	_snowbuffer[1].genTextures();
	_snowbuffer[1].setFormat(GL_RG32F, GL_RED, GL_FLOAT);

	_noisebuffer[0].init(1);
	_noisebuffer[0].genTextures();
	_noisebuffer[0].setFormat(GL_R32F, GL_RED, GL_FLOAT);

	_noisebuffer[1].init(1);
	_noisebuffer[1].genTextures();
	_noisebuffer[1].setFormat(GL_R32F, GL_RED, GL_FLOAT);

	_framebuffer[0].init();
	_framebuffer[1].init();
	_erosionbuffer[0].init();
	_erosionbuffer[1].init();

	_copybuffer.resize(_noise_params->resolution, _noise_params->resolution);

	resize();

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

	GLuint* tex_height;

	const GLuint TEX_HEIGHT_INDEX = 0;

	CLEAR_BUFFERS(_noisebuffer);
	SET_MODE(NOISE_MODE);

	NoiseParams noise_params_tmp = _noise_params->copy();
	noise_params_tmp.amplitude = 1;

	// Two variables to swap buffers
	INIT(in, out);
	SWAP_TEXTURE(tex_height, _noisebuffer, TEX_HEIGHT_INDEX);

	if (_noise_params->fractal_type == MULTIFRACTAL) {
		_noisebuffer[out].bind(BUFFER_ATTACHMENT_0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.drawNoise(&FLAT_NOISE, 1, tex_height);
		_noisebuffer[out].unbind();

		SWAP(in, out);
		SWAP_TEXTURE(tex_height, _noisebuffer, TEX_HEIGHT_INDEX);
	}

	for (int i = 0; i < _noise_params->octaves; i++) {

		_noisebuffer[out].bind(BUFFER_ATTACHMENT_0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.drawNoise(&noise_params_tmp, noise_params_tmp.amplitude, tex_height);
		_noisebuffer[out].unbind();

		noise_params_tmp.height *= _noise_params->lacunarity;
		noise_params_tmp.width *= _noise_params->lacunarity;
		noise_params_tmp.amplitude *= pow(_noise_params->lacunarity, -_noise_params->H);

		// Swap input & output textures
		SWAP(in, out);
		SWAP_TEXTURE(tex_height, _noisebuffer, TEX_HEIGHT_INDEX);
	}

	copyNoise(tex_height, _tex_height, _noise_params->amplitude, _noise_params->offset);
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

void NoiseGenerator::resize() {
	_snowbuffer[0].setSize(_noise_params->resolution, _noise_params->resolution);
	_snowbuffer[0].genTextureImages();
	_snowbuffer[0].wrap(BUFFER_ATTACHMENT_2, 3);
	_snowbuffer[0].clear();

	_snowbuffer[1].setSize(_noise_params->resolution, _noise_params->resolution);
	_snowbuffer[1].genTextureImages();
	_snowbuffer[1].wrap(BUFFER_ATTACHMENT_2, 3);
	_snowbuffer[1].clear();

	_noisebuffer[0].setSize(_noise_params->resolution, _noise_params->resolution);
	_noisebuffer[0].genTextureImages();
	_noisebuffer[0].wrap(BUFFER_ATTACHMENT_0, 1);
	_noisebuffer[0].clear();

	_noisebuffer[1].setSize(_noise_params->resolution, _noise_params->resolution);
	_noisebuffer[1].genTextureImages();
	_noisebuffer[1].wrap(BUFFER_ATTACHMENT_0, 1);
	_noisebuffer[1].clear();

}

void NoiseGenerator::addSnow() {
	GLuint* tex_snow;
	GLuint* tex_height;
	GLuint* tex_pos;

	const GLuint TEX_HEIGHT_INDEX = 0;
	const GLuint TEX_SNOW_INDEX = 1;
	const GLuint TEX_POS_INDEX = 2;

	CLEAR_BUFFERS(_snowbuffer);
	SET_MODE(SNOW_MODE);

	if (_snow_params->enable) {

		INIT(in, out);

		tex_height = _tex_height;
		SWAP_TEXTURE(tex_snow, _snowbuffer, TEX_SNOW_INDEX);
		SWAP_TEXTURE(tex_pos, _snowbuffer, TEX_POS_INDEX);

		_snowbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.fall(tex_height, tex_snow, tex_pos);
		_snowbuffer[out].unbind();

		SWAP(in, out);
		SWAP_TEXTURE(tex_height, _snowbuffer, TEX_HEIGHT_INDEX);
		SWAP_TEXTURE(tex_snow, _snowbuffer, TEX_SNOW_INDEX);
		SWAP_TEXTURE(tex_pos, _snowbuffer, TEX_POS_INDEX);
		
		for (int t = 0; t < _snow_params->slide_time; t++) {

			_snowbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.slide(tex_height, tex_snow, tex_pos);
			_snowbuffer[out].unbind();

			SWAP(in, out);
			SWAP_TEXTURE(tex_height, _snowbuffer, TEX_HEIGHT_INDEX);
			SWAP_TEXTURE(tex_snow, _snowbuffer, TEX_SNOW_INDEX);
			SWAP_TEXTURE(tex_pos, _snowbuffer, TEX_POS_INDEX);
		}
		
		for (int t = 0; t < _snow_params->melt_time; t++) {

			_snowbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.melt(tex_height, tex_snow, tex_pos);
			_snowbuffer[out].unbind();

			SWAP(in, out);
			SWAP_TEXTURE(tex_height, _snowbuffer, TEX_HEIGHT_INDEX);
			SWAP_TEXTURE(tex_snow, _snowbuffer, TEX_SNOW_INDEX);
			SWAP_TEXTURE(tex_pos, _snowbuffer, TEX_POS_INDEX);
		}

		for (int t = 0; t < _snow_params->smooth_time; t++) {

			_snowbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
				glClear(GL_COLOR_BUFFER_BIT);
				_quad.smooth(tex_height, tex_snow, tex_pos);
			_snowbuffer[out].unbind();

			SWAP(in, out);
			SWAP_TEXTURE(tex_height, _snowbuffer, TEX_HEIGHT_INDEX);
			SWAP_TEXTURE(tex_snow, _snowbuffer, TEX_SNOW_INDEX);
			SWAP_TEXTURE(tex_pos, _snowbuffer, TEX_POS_INDEX);
		}

		copyTexture(tex_height, _tex_height);
		copyTexture(tex_snow, _tex_snow);
	}
}

void NoiseGenerator::copyTexture(GLuint* src, GLuint* dst) {
	_copybuffer.resize(_noise_params->resolution, _noise_params->resolution);
	_copybuffer.init(dst);

	_quad.setShaders(COPY_MODE);
	_quad.genVertexArray();
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
