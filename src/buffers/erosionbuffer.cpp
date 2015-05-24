#include "erosionbuffer.h"

Erosionbuffer::Erosionbuffer(){}

void Erosionbuffer::bind() {
	glViewport(0, 0, _width, _height);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	const GLenum buffers[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};

	glDrawBuffers(3, buffers);
}

void Erosionbuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Erosionbuffer::clear() {
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		glClearColor(.0, .0, .0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(.937, .937, .937, 1.0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Erosionbuffer::init() {

	// Create all textures
	{
		glGenTextures(1, &_tex_height);
		glBindTexture(GL_TEXTURE_2D, _tex_height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, _width, _height, 0, GL_RED, GL_FLOAT, NULL);

		glGenTextures(1, &_tex_snow);
		glBindTexture(GL_TEXTURE_2D, _tex_snow);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, _width, _height, 0, GL_RED, GL_FLOAT, NULL);

		glGenTextures(1, &_tex_pos);
		glBindTexture(GL_TEXTURE_2D, _tex_pos);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, _width, _height, 0, GL_RED, GL_FLOAT, NULL);
	}

	///--- Tie it all together
	{
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, _fbo);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex_height, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _tex_snow, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _tex_pos, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cerr << "!!!ERROR: Framebuffer not OK :(" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0); ///< avoid pollution
	}
}

void Erosionbuffer::resize(float width, float height) {
	cleanup();
	_width = width;
	_height = height;
	init();
}

void Erosionbuffer::cleanup() {
	glDeleteTextures(1, &_tex_height);
	glDeleteTextures(1, &_tex_snow);
	glDeleteTextures(1, &_tex_pos);
	glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
	glDeleteFramebuffers(1, &_fbo);
}

GLuint* Erosionbuffer::get_tex_height() {
	return &_tex_height;
}

GLuint* Erosionbuffer::get_tex_snow() {
	return &_tex_snow;
}

GLuint* Erosionbuffer::get_tex_pos() {
	return &_tex_pos;
}
