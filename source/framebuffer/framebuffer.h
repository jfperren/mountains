#pragma once
#include "icg_common.h"

#ifdef WITH_OPENCV
#include "opencv2/core/core.hpp"       ///< cv::Mat
#include "opencv2/highgui/highgui.hpp" ///< cv::imShow
#include "opencv2/contrib/contrib.hpp" ///< cvtConvert
#include "opencv2/imgproc/types_c.h"   ///< CV_BGRA2RGBA
#endif

class Framebuffer{

protected:
	bool _init;
	int _width;
	int _height;
	GLuint _fbo;

	GLuint* _tex;

	GLint _internal_format;
	GLenum _format;
	GLenum _type;

public:

	Framebuffer(){ }

	Framebuffer(int image_width, int image_height){
		this->_width = image_width;
		this->_height = image_height;
	}

	///--- Warning: ovverrides viewport!!
	void bind() {
		glViewport(0, 0, _width, _height);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1 /*length of buffers[]*/, buffers);
	}

	void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void clear() {
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		glClearColor( .0, .0, .0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(.937, .937, .937, 1.0);
	}

	int init_texture(bool use_interpolation = false) {

		_tex = (GLuint*) malloc(sizeof(GLuint));

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

	void set_texture(GLuint* texture, bool use_interpolation = false) {
		_tex = texture;
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
	}

	void init(GLint internal_format, GLenum format, GLenum type) {

		_internal_format = internal_format;
		_format = format;
		_type = type;

		///--- Create texture for the color attachment
		/// See Table.2 https://www.khronos.org/opengles/sdk/docs/man3/docbook4/xhtml/glTexImage2D.xml
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format,
			_width, _height, 0,
			format, type, NULL); ///< how to load from buffer

		///--- Tie it all together
		{
			glGenFramebuffers(1, &_fbo);
			glBindFramebuffer(GL_FRAMEBUFFER_EXT, _fbo);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 /*location = 0*/, GL_TEXTURE_2D, *_tex, 0 /*level*/);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cerr << "!!!ERROR: Framebuffer not OK :(" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0); ///< avoid pollution
		}
	}

	void resize(float width, float height) {
		_width = width;
		_height = height;
	}

	void cleanup() {
		glDeleteTextures(1, _tex);
		glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
		glDeleteFramebuffers(1, &_fbo);
	}
};
