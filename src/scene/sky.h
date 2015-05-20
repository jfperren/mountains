#pragma once

#include "icg_common.h"

class Sky {
	private:
		GLuint _tex_skybox[6];
		GLuint _vao;
		GLuint _vbo;
		GLuint _pid;

		mat4 model = mat4::Identity();
		
	public:
		void init();
		void cleanup();
		void draw(const mat4& view, const mat4& projection);
};