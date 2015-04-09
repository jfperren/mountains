#pragma once

#include "icg_common.h"
#include "../framebuffer/FrameBuffer.h"
#include "../fullscreenquad/FullScreenQuad.h"

class NoiseGenerator {

	FullScreenQuad fsQuad;

public:
	void init() {
		fsQuad.init();
	}

	void createRandomNoise(int width, int height, float min_value, float max_value, GLuint* texture) {

		// Create framebuffer with texture as output
		FrameBuffer fb(width, height);
		fb.initWithTexture(texture);

		///--- Render random noise on fullScreenQuad in the framebuffer
		fb.bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			fsQuad.drawNoise(width, height, min_value, max_value);
		fb.unbind();
	}
};