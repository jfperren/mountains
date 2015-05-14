#include "depthbuffer.h"

Depthbuffer::Depthbuffer(int image_width, int image_height){
	this->_width = image_width;
	this->_height = image_height;
}

///--- Warning: ovverrides viewport!!
void Depthbuffer::bind() {
	glViewport(0, 0, _width, _height);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	const GLenum buffers[] = { GL_DEPTH_ATTACHMENT };
	glDrawBuffers(1 /*length of buffers[]*/, buffers);
}

void Depthbuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int Depthbuffer::init_texture(bool use_interpolation) {

	_tex = (GLuint*)malloc(sizeof(GLuint));

	glGenTextures(1, _tex);
	glBindTexture(GL_TEXTURE_2D, *_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (use_interpolation){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	return *_tex;
}

void Depthbuffer::init() {
	///--- Create texture for the color attachment
	/// See Table.2 https://www.khronos.org/opengles/sdk/docs/man3/docbook4/xhtml/glTexImage2D.xml
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		_width, _height, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL); ///< how to load from buffer

	///--- Tie it all together
	{
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, _fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *_tex, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cerr << "!!!ERROR: Framebuffer not OK :(" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0); ///< avoid pollution
	}
}

void Depthbuffer::resize(float width, float height) {
	_width = width;
	_height = height;
}

void Depthbuffer::cleanup() {
	glDeleteTextures(1, _tex);
	glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
	glDeleteFramebuffers(1, &_fbo);
}