#pragma once

#include "icg_common.h"
#include "../framebuffer/Framebuffer.h"
#include "../noise/NoiseQuad.h"
#include "../constants.h"

void renderNoise(GLuint* out_texture, NoiseValues noise_values, GLuint* in_texture = nullptr, int aggregation_type = FBM) {

	NoiseQuad quad;
	quad.init();

	// Create framebuffer with texture as output
	Framebuffer fb(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
	fb.initWithTexture(out_texture);

	///--- Render random noise on quad in the framebuffer
	fb.bind();
	glClear(GL_COLOR_BUFFER_BIT);
	quad.drawNoise(noise_values, in_texture, aggregation_type);
	fb.unbind();
}

void renderFractal(GLuint* out_texture, NoiseValues noise_values, FractalValues fractal_values) {
		// To avoid strange behaviours, should put noise_values.offset to 0;
		
		if (fractal_values.octaves < 1) {
			return;
		}

		GLuint textures[2];

		if (fractal_values.fractal_type == MULTIFRACTAL) {
			// Start with height 1 otherwise it will multiply 0 and render nothing
			renderNoise(&textures[0], NoiseValues{ NO_NOISE, NO_EFFECT, 1, 1, 0, 1 });
		}

		int inputtexture = 0;
		for (int i = 0; i < fractal_values.octaves; i++) {

			renderNoise(&(textures[1 - inputtexture]), noise_values, &(textures[inputtexture]), fractal_values.fractal_type);

			noise_values.height *= fractal_values.lacunarity;
			noise_values.width *= fractal_values.lacunarity;
			noise_values.amplitude *= pow(fractal_values.lacunarity, -fractal_values.H);

			noise_values.seed = noise_values.seed * 19.548978;
			noise_values.seed -= floor(noise_values.seed);

			// Swap input & output textures
			inputtexture = 1 - inputtexture;
		}

		// Render created texture in out_texture with offset and amplitude
		renderNoise(out_texture, NoiseValues{ COPY_TEXTURE, fractal_values.fractal_effect, 1, 1, fractal_values.amplitude, fractal_values.offset }, &(textures[inputtexture]));
}