#pragma once
#include "icg_common.h"

#ifdef WITH_OPENCV
#include "opencv2/core/core.hpp"       ///< cv::Mat
#include "opencv2/highgui/highgui.hpp" ///< cv::imShow
#include "opencv2/contrib/contrib.hpp" ///< cvtConvert
#include "opencv2/imgproc/types_c.h"   ///< CV_BGRA2RGBA
#endif

class Depthbuffer{

protected:
	bool _init;
	int _width;
	int _height;
	GLuint _fbo;

	GLuint* _tex;

public:
	Depthbuffer(int image_width, int image_height);

	int init_texture(bool use_interpolation = false);
	void init();

	void bind();
	void unbind();
	
	void resize(float width, float height);

	void cleanup();
};
