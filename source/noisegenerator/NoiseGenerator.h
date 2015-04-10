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
	static void NoiseGenerator::renderNoise(int noise_type, int width, int height, float offset, float amplitude, GLuint* texture) {

		FullScreenQuad fsQuad;
		fsQuad.init();

		// Create framebuffer with texture as output
		FrameBuffer fb(PIXELS_PER_UNIT, PIXELS_PER_UNIT);
		fb.initWithTexture(texture);

		///--- Render random noise on fullScreenQuad in the framebuffer
		fb.bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			fsQuad.drawNoise(noise_type, width, height, offset, amplitude, nullptr);
		fb.unbind();
	}

	static void NoiseGenerator::generateFractionalBrownianMotion(GLuint* texture, float amplitude,
		float H, float lacunarity, int octaves) {
		
		FullScreenQuad fsQuad;
		fsQuad.init();

		GLuint textures[2];

		FrameBuffer framebuffers[] = {
			FrameBuffer(500, 500), 
			FrameBuffer(500, 500)
		};

		framebuffers[0].initWithTexture(&textures[0]);
		framebuffers[1].initWithTexture(&textures[1]);

		int currentbuffer = 0;
		for (int i = 0; i < octaves; i++) {
			if (i != octaves - 1) {
				// Render into framebuffer "currentbuffer" (= into texture "currentbuffer")
				framebuffers[currentbuffer].bind();
				glClear(GL_COLOR_BUFFER_BIT);

				// Using texture "1 - currentbuffer"
				fsQuad.drawNoise(FullScreenQuad::PERLIN_NOISE,pow(lacunarity, i), pow(lacunarity, i), 0, amplitude * pow(lacunarity, -i * H), &(textures[1 - currentbuffer]));

				// Unbind
				framebuffers[currentbuffer].unbind();

				// Alternate buffers
				currentbuffer = 1 - currentbuffer;
			} else {
				// Last iteration should be rendered into the texture given as parameter
				FrameBuffer fb(500, 500);

				fb.initWithTexture(texture);
				fb.bind();
				fsQuad.drawNoise(FullScreenQuad::PERLIN_NOISE, pow(lacunarity, i), pow(lacunarity, i), 0, amplitude * pow(lacunarity, -i * H), &(textures[1 - currentbuffer]));
				fb.unbind();
			}
		}
	}
};