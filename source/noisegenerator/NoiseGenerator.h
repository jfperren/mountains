#pragma once

#include "icg_common.h"
#include "../framebuffer/FrameBuffer.h"
#include "../fullscreenquad/FullScreenQuad.h"

class NoiseGenerator {

	//typedef void(*NoiseFunction)(int, int, float, float, GLuint*);

public:
	const static int PIXELS_PER_UNIT = 500;
protected:
	

public:
	static void NoiseGenerator::renderNoise(int noise_type, int width, int height, float offset, float amplitude, GLuint* texture, GLuint* input = nullptr) {

		FullScreenQuad fsQuad;
		fsQuad.init();

		// Create framebuffer with texture as output
		FrameBuffer fb(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
		fb.initWithTexture(texture);

		///--- Render random noise on fullScreenQuad in the framebuffer
		fb.bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			fsQuad.drawNoise(noise_type, width, height, offset, amplitude, input);
		fb.unbind();
	}

	static void NoiseGenerator::generateFractionalBrownianMotion(GLuint* texture, float amplitude,
		float H, float lacunarity, int octaves) {
		
		GLuint textures[2];

		// Render offset in texture[1]
		renderNoise(FullScreenQuad::NO_NOISE, 1, 1, 1, 0, &textures[1]);

		int currentbuffer = 0;
		for (int i = 0; i < octaves - 1; i++) {
			renderNoise(FullScreenQuad::PERLIN_NOISE, pow(lacunarity, i), pow(lacunarity, i), 0, amplitude * pow(lacunarity, -i * H), &(textures[1 - currentbuffer]), &(textures[currentbuffer]));
			currentbuffer = 1 - currentbuffer;
		}

		renderNoise(FullScreenQuad::PERLIN_NOISE, pow(lacunarity, octaves - 1), pow(lacunarity, octaves - 1), 0, amplitude * pow(lacunarity, -(octaves - 1) * H), texture, &(textures[currentbuffer]));
	}
};