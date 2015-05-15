#include "noise_quad.h"

void NoiseQuad::init(){

	///--- Compile the shaders
	/*_pid = opengp::load_shaders("noise/noise_quad_vshader.glsl", "noise/noise_quad_fshader.glsl");
	if (!_pid) exit(EXIT_FAILURE);
	glUseProgram(_pid);*/

	///--- Vertex one vertex Array
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	///--- Vertex coordinates
	{
		const GLfloat vpoint[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
			/*V2*/ +1.0f, -1.0f, 0.0f,
			/*V3*/ -1.0f, +1.0f, 0.0f,
			/*V4*/ +1.0f, +1.0f, 0.0f };
		///--- Buffer
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

		///--- Attribute
		GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
		glEnableVertexAttribArray(vpoint_id);
		glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
	}

	///--- to avoid the current object being polluted
	glBindVertexArray(0);
	glUseProgram(0);
}

void NoiseQuad::cleanup(){
	// TODO cleanup
}

void NoiseQuad::drawNoise(NoiseParams* noise_params, float noise_amplitude, GLuint* in_texture){

	// Bind program & vertex array
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	// Tells shader we want to draw
	glUniform1i(glGetUniformLocation(_pid, "draw_or_copy"), DRAW_NOISE);

	noise_params->setup_draw(_pid);

	glUniform1f(glGetUniformLocation(_pid, "noise_amplitude"), noise_amplitude);
	glUniform1f(glGetUniformLocation(_pid, "noise_offset"), noise_params->fractal_type == MULTIFRACTAL ? 1 : 0);

	if (in_texture != nullptr) {
		// Tells shader there is a texture as input
		glUniform1i(glGetUniformLocation(_pid, "is_texture"), 1);

		// Set texture input
		glUniform1i(glGetUniformLocation(_pid, "tex"), 0 /*GL_TEXTURE0*/);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *in_texture);
	}

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Unbind
	glBindVertexArray(0);
	glUseProgram(0);
}

void NoiseQuad::copyNoise(NoiseParams* noise_params, GLuint* in_texture){
	// Bind program & vertex array
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	// Tells shader we want to copy
	glUniform1i(glGetUniformLocation(_pid, "draw_or_copy"), COPY_NOISE);

	noise_params->setup_copy(_pid);

	// Set texture input
	glUniform1i(glGetUniformLocation(_pid, "tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *in_texture);

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Unbind
	glBindVertexArray(0);
	glUseProgram(0);
}