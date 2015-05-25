#pragma once

#include "icg_common.h"
#include "../app/constants.h"

class Sky {
	private:
		GLuint _tex_skybox[6];
		GLuint _vao;
		GLuint _vbo;
		GLuint _pid;

		mat4 model = mat4::Identity();
		
	public:
		void init(ThemeType theme_type);
		void cleanup();
		void draw(const mat4& view, const mat4& projection);
};