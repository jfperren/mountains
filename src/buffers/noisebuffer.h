#pragma once

#include "icg_common.h"

#ifdef WITH_OPENCV
#include "opencv2/core/core.hpp"       ///< cv::Mat
#include "opencv2/highgui/highgui.hpp" ///< cv::imShow
#include "opencv2/contrib/contrib.hpp" ///< cvtConvert
#include "opencv2/imgproc/types_c.h"   ///< CV_BGRA2RGBA
#endif

class Noisebuffer{

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
	Noisebuffer();
	
	void bind();

	void unbind();
	void cleanup();

	int init_texture();
	void init();
	GLuint* get_tex();
	void resize(float width, float height);
	void clear();
};
