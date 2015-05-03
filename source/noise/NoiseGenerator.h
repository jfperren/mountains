#pragma once

#include "icg_common.h"
#include "../framebuffer/frame_buffer_generic.h"
#include "../noise/NoiseQuad.h"
#include "../constants.h"

void renderNoise(GLuint* out_texture, NoiseParams* noise_params, GLuint* in_texture = nullptr, int aggregation_type = FBM) {

	NoiseQuad quad;
	quad.init();

	// Create framebuffer with texture as output
	FB fb(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
	fb.set_texture(out_texture);
	fb.init(GL_R32F, GL_RED, GL_FLOAT);


	///--- Render random noise on quad in the framebuffer
	fb.bind();
		glClear(GL_COLOR_BUFFER_BIT);
		quad.drawNoise(noise_params, in_texture, aggregation_type);
	fb.unbind();
}

void renderFractal(GLuint* out_texture, NoiseParams* noise_params, FractalParams* fractal_params) {
		// To avoid strange behaviours, should put noise_params.offset to 0;
		
		NoiseParams noise_params_2 = *noise_params;

		if (fractal_params->octaves < 1) {
			return;
		}

		GLuint textures[2];

		if (fractal_params->fractal_type == MULTIFRACTAL) {
			NoiseParams flat_noise = NoiseParams{ NO_NOISE, NO_EFFECT, 1, 1, 0, 1 };
			// Start with height 1 otherwise it will multiply 0 and render nothing
			renderNoise(&textures[0], &flat_noise);
		}

		int inputtexture = 0;
		for (int i = 0; i < fractal_params->octaves; i++) {

			renderNoise(&(textures[1 - inputtexture]), &noise_params_2, &(textures[inputtexture]), fractal_params->fractal_type);

			noise_params_2.height *= fractal_params->lacunarity;
			noise_params_2.width *= fractal_params->lacunarity;
			noise_params_2.amplitude *= pow(fractal_params->lacunarity, -fractal_params->H);

			//noise_params->seed = noise_params->seed * 19.548978;
			//noise_params->seed -= floor(noise_params->seed);

			// Swap input & output textures
			inputtexture = 1 - inputtexture;
		}

		// Render created texture in out_texture with offset and amplitude
		NoiseParams fractal_noise = NoiseParams{ COPY_TEXTURE, fractal_params->fractal_effect, 1, 1, fractal_params->amplitude, fractal_params->offset };
		renderNoise(out_texture, &fractal_noise, &(textures[inputtexture]));
}