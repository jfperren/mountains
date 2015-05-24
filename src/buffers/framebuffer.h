#pragma once

#include "icg_common.h"

#ifdef WITH_OPENCV
#include "opencv2/core/core.hpp"       ///< cv::Mat
#include "opencv2/highgui/highgui.hpp" ///< cv::imShow
#include "opencv2/contrib/contrib.hpp" ///< cvtConvert
#include "opencv2/imgproc/types_c.h"   ///< CV_BGRA2RGBA
#endif

class Framebuffer{

private:
	bool _init;
	int _width;
	int _height;
	GLuint _fbo;

	GLuint* _tex;

	GLint _internal_format;
	GLenum _format;
	GLenum _type;

public:
	Framebuffer();
	Framebuffer(int image_width, int image_height);

	void bind();
	void unbind();
	void cleanup();

	int init_texture(bool use_interpolation = false);
	void set_texture(GLuint* texture, bool use_interpolation = false);
	void init(GLint internal_format, GLenum format, GLenum type);

	void resize(float width, float height);
	void clear();

};