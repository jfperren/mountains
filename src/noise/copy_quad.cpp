#include "copy_quad.h"

void CopyQuad::init(){

	///--- Compile the shaders
	_pid = opengp::load_shaders("noise/quad_copy_vshader.glsl", "noise/copy_quad_fshader.glsl");
	if (!_pid) exit(EXIT_FAILURE);
	glUseProgram(_pid);

	///--- Vertex one vertex Array
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	///--- Vertex coordinates
	{
		const GLfloat vertex_pos_array[] = {
			-1.0f, -1.0f, 0.0f,
			+1.0f, -1.0f, 0.0f,
			-1.0f, +1.0f, 0.0f,
			+1.0f, +1.0f, 0.0f
		};

		///--- Buffer
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_pos_array), vertex_pos_array, GL_STATIC_DRAW);

		///--- Attribute
		GLuint vertex_pos_id = glGetAttribLocation(_pid, "vertex_pos");
		glEnableVertexAttribArray(vertex_pos_id);
		glVertexAttribPointer(vertex_pos_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
	}

	///--- to avoid the current object being polluted
	glBindVertexArray(0);
	glUseProgram(0);
}

void CopyQuad::cleanup(){
	// TODO cleanup
}

