#include "erosion_quad.h"

void ErosionQuad::init(DirtParams* dirt_params){

	_dirt_params = dirt_params;

	///--- Compile the shaders
	_pid = opengp::load_shaders("noise/erosion_quad_vshader.glsl", "noise/erosion_quad_fshader.glsl");
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

void ErosionQuad::cleanup(){
	// TODO cleanup
}

void ErosionQuad::draw(GLuint* tex_height, GLuint* tex_water, GLuint* tex_sediment, GLuint* tex_pos){

	// Bind program & vertex array
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	_dirt_params->setup(_pid);

	glUniform1f(glGetUniformLocation(_pid, "DX"), 1.0/2048);
	glUniform1f(glGetUniformLocation(_pid, "DY"), 1.0/2048);
	glUniform1f(glGetUniformLocation(_pid, "DZ"), sqrt(2) / 2048.0);

	// Set texture input
	glUniform1i(glGetUniformLocation(_pid, "tex_in_height"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *tex_height);

	glUniform1i(glGetUniformLocation(_pid, "tex_in_water"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *tex_water);

	glUniform1i(glGetUniformLocation(_pid, "tex_in_sediment"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, *tex_sediment);

	glUniform1i(glGetUniformLocation(_pid, "tex_in_pos"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, *tex_pos);
	
	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Unbind
	glBindVertexArray(0);
	glUseProgram(0);
}