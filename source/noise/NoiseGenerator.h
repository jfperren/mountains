#pragma once

#include "icg_common.h"
#include "../framebuffer/Framebuffer.h"
#include "../noise/NoiseQuad.h"

class NoiseGenerator {

public:
	const static int PIXELS_PER_UNIT = 2048;
protected:
	

public:
	static void NoiseGenerator::renderNoise(GLuint* out_texture, NoiseQuad::NoiseValues noise_values, GLuint* in_texture = nullptr, int aggregation_type = NoiseQuad::FBM) {

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

	static void NoiseGenerator::generateFBM(GLuint* out_texture, NoiseQuad::NoiseValues noise_values, float amplitude, float offset,
		float H, float lacunarity, int octaves) {
		// To avoid strange behaviours, should put noise_values.offset to 0;
		
		if (octaves < 1) {
			return;
		}

		GLuint textures[2];

		int inputtexture = 0;
		for (int i = 0; i < octaves; i++) {

			renderNoise(&(textures[1 - inputtexture]), noise_values, &(textures[inputtexture]), NoiseQuad::FBM);

			noise_values.height *= lacunarity;
			noise_values.width *= lacunarity;
			noise_values.amplitude *= pow(lacunarity, -H);

			noise_values.seed = noise_values.seed * 8509473409850.5489;
			noise_values.seed -= floor(noise_values.seed);

			// Swap input & output textures
			inputtexture = 1 - inputtexture;
			
		}

		// Render created texture in out_texture with offset and amplitude
		renderNoise(out_texture, NoiseQuad::NoiseValues{ NoiseQuad::COPY_TEXTURE, 1, 1, amplitude, offset}, &(textures[inputtexture]));
	}

	static void NoiseGenerator::generateMultiFractal(GLuint* out_texture, NoiseQuad::NoiseValues noise_values, float amplitude, float offset,
		float H, float lacunarity, int octaves) {

		if (octaves < 1) {
			return;
		}

		GLuint textures[2];

		// Start with height 1 otherwise it will multiply 0 and render nothing
		renderNoise(&textures[0], NoiseQuad::NoiseValues{ NoiseQuad::NO_NOISE, 1, 1, 0, 1});

		int currentbuffer = 0;
		for (int i = 0; i < octaves; i++) {
			
			renderNoise(&(textures[1 - currentbuffer]), noise_values , &(textures[currentbuffer]), NoiseQuad::MULTIFRACTAL);

			noise_values.height *= lacunarity;
			noise_values.width *= lacunarity;
			noise_values.amplitude *= pow(lacunarity, -H);
			
			noise_values.seed = noise_values.seed * 8509473409850.5489;
			noise_values.seed -= floor(noise_values.seed);

			currentbuffer = 1 - currentbuffer;
		}

		// Render created texture in out_texture with offset and amplitude
		renderNoise(out_texture, NoiseQuad::NoiseValues{ NoiseQuad::COPY_TEXTURE, 1, 1, amplitude, offset }, &(textures[currentbuffer]), NoiseQuad::FBM);
	}
};