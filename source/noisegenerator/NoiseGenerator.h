#pragma once

#include "icg_common.h"
#include "../framebuffer/FrameBuffer.h"
#include "../fullscreenquad/FullScreenQuad.h"

class NoiseGenerator {

public:
	const static int PIXELS_PER_UNIT = 500;
protected:
	

public:
	static void NoiseGenerator::createRandomNoise(int width, int height, float min_value, float max_value, GLuint* texture) {

		FullScreenQuad fsQuad;
		fsQuad.init();

		// Create framebuffer with texture as output
		FrameBuffer fb(width, height);
		fb.initWithTexture(texture);

		///--- Render random noise on fullScreenQuad in the framebuffer
		fb.bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			fsQuad.drawRandomNoise(width, height, min_value, max_value);
		fb.unbind();
	}

	static void NoiseGenerator::createPerlinNoise(int width, int height, float min_value, float max_value, GLuint* texture) {

		FullScreenQuad fsQuad;
		fsQuad.init();

		// Create framebuffer with texture as output
		FrameBuffer fb(width * PIXELS_PER_UNIT, height * PIXELS_PER_UNIT);
		fb.initWithTexture(texture);

		///--- Render random noise on fullScreenQuad in the framebuffer
		fb.bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			fsQuad.drawPerlinNoise(width, height, min_value, max_value);
		fb.unbind();
	}
};