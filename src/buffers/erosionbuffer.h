#pragma once

#include "icg_common.h"

#ifdef WITH_OPENCV
#include "opencv2/core/core.hpp"       ///< cv::Mat
#include "opencv2/highgui/highgui.hpp" ///< cv::imShow
#include "opencv2/contrib/contrib.hpp" ///< cvtConvert
#include "opencv2/imgproc/types_c.h"   ///< CV_BGRA2RGBA
#endif

class Erosionbuffer{

private:
	int _width;
	int _height;

	GLuint _fbo;
	
	GLuint _tex_height;
	GLuint _tex_water;
	GLuint _tex_sediment;
	GLuint _tex_pos;

public:
	Erosionbuffer();

	void init();
	void bind();
	void unbind();
	void clear();
	void cleanup();

	GLuint* get_tex_height();
	GLuint* get_tex_water();
	GLuint* get_tex_sediment();
	GLuint* get_tex_pos();

	void resize(float width, float height);
};
