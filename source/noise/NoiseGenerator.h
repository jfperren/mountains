#pragma once

#include "icg_common.h"
#include "../framebuffer/framebuffer.h"
#include "../noise/NoiseQuad.h"
#include "../constants.h"

class NoiseGenerator {

private:

	// 0 = out, 1/2 = tmp
	FB _framebuffer[3];
	GLuint* _tex[3];

	NoiseQuad _quad;
	NoiseParams* _noise_params;
	FractalParams* _fractal_params;

public:
	NoiseGenerator(GLuint* out_tex, NoiseParams* noise_params, FractalParams* fractal_params) :
		_noise_params(noise_params),
		_fractal_params(fractal_params)
	{ 
		_tex[0] = (GLuint*) malloc(sizeof(GLuint));
		_tex[1] = (GLuint*) malloc(sizeof(GLuint));
		_tex[2] = (GLuint*) malloc(sizeof(GLuint));

		_tex[0] = out_tex;
	}

	void init() {
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

	void renderNoise(int out, int in, NoiseParams* noise_params, int aggregation_type = FBM) {
		///--- Render random noise on quad in the framebuffer
		_framebuffer[out].bind();
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.drawNoise(noise_params, _tex[in], aggregation_type);
		_framebuffer[out].unbind();
	}

	void renderFractal() {

		_framebuffer[0].clear();
		_framebuffer[1].clear();
		_framebuffer[2].clear();

		if (!_fractal_params->enable || _fractal_params->octaves == 1) {
			///--- Render random noise on quad in the framebuffer
			_framebuffer[0].bind();
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.drawNoise(_noise_params);
			_framebuffer[0].unbind();
		} else {
			NoiseParams noise_params_tmp = {
				_noise_params->type,
				_noise_params->effect,
				_noise_params->width,
				_noise_params->height,
				_noise_params->amplitude,
				_noise_params->offset,
				_noise_params->seed
			};

			// Two variables to swap buffers
			int in = 1;
			int out = 2;

			// Clean first input
			if (_fractal_params->fractal_type == MULTIFRACTAL) {
				NoiseParams flat_noise = NoiseParams{ NO_NOISE, NO_EFFECT, 1, 1, 0, 1 };
				renderNoise(in, out, &flat_noise);
			}

			for (int i = 0; i < _fractal_params->octaves; i++) {
				cout << "iteration" << endl;
				cout << "height : " << noise_params_tmp.height << endl;
				cout << "width : " << noise_params_tmp.width << endl;
				cout << "amplitude : " << noise_params_tmp.amplitude << endl;
				cout << "in" << in << endl;
				cout << "out" << out << endl;

				renderNoise(out, in, &noise_params_tmp, _fractal_params->fractal_type);

				noise_params_tmp.height *= _fractal_params->lacunarity;
				noise_params_tmp.width *= _fractal_params->lacunarity;
				noise_params_tmp.amplitude *= pow(_fractal_params->lacunarity, -_fractal_params->H);

				// Swap input & output textures
				in = (in == 1 ? 2 : 1);
				out = (out == 1 ? 2 : 1);
			}

			// Render created texture in out_texture with offset and amplitude
			NoiseParams copy_noise = NoiseParams{ COPY_TEXTURE, _fractal_params->fractal_effect, 1, 1, _fractal_params->amplitude, _fractal_params->offset };
			renderNoise(0, in, &copy_noise);
		}
	}
};