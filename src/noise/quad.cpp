#include "quad.h"

void Quad::init(AppParams* app_params){
	_snow_params = app_params->snow_params;
	_grass_params = app_params->grass_params;
}

void Quad::setShaders(int mode) {

	///--- Compile the shaders
	if (mode == NOISE_MODE) {
		_pid = opengp::load_shaders("noise/quad_noise_vshader.glsl", "noise/quad_noise_fshader.glsl");
	} else if (mode == SNOW_MODE) {
		_pid = opengp::load_shaders("noise/quad_snow_vshader.glsl", "noise/quad_snow_fshader.glsl");
	} else if (mode == GRASS_MODE) {
	 _pid = opengp::load_shaders("noise/quad_grass_vshader.glsl", "noise/quad_grass_fshader.glsl");
	} else if (mode == COPY_MODE){
		_pid = opengp::load_shaders("noise/quad_copy_vshader.glsl", "noise/quad_copy_fshader.glsl");
	}

	if (!_pid) exit(EXIT_FAILURE);
}

void Quad::genVertexArray() {

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

void Quad::cleanup(){
	// TODO cleanup
}

void Quad::fall(GLuint* tex_height, GLuint* tex_snow, GLuint* tex_pos) {
	this->drawSnow(tex_height, tex_snow, tex_pos, FALL);
}

void Quad::slide(GLuint* tex_height, GLuint* tex_snow, GLuint* tex_pos) {
	this->drawSnow(tex_height, tex_snow, tex_pos, SLIDE);
}

void Quad::melt(GLuint* tex_height, GLuint* tex_snow, GLuint* tex_pos) {
	this->drawSnow(tex_height, tex_snow, tex_pos, MELT);
}

void Quad::smooth(GLuint* tex_height, GLuint* tex_snow, GLuint* tex_pos) {
	this->drawSnow(tex_height, tex_snow, tex_pos, SMOOTH);
}

void Quad::drawNoise(NoiseParams* noise_params, float noise_amplitude, GLuint* in_texture){

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

void Quad::drawSnow(GLuint* tex_height, GLuint* tex_snow, GLuint* tex_pos, int action){

	// Bind program & vertex array
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	_snow_params->setup(_pid);

	glUniform1i(glGetUniformLocation(_pid, "action"), action);

	glUniform1f(glGetUniformLocation(_pid, "DX"), 1.0 / 2048);
	glUniform1f(glGetUniformLocation(_pid, "DY"), 1.0 / 2048);
	glUniform1f(glGetUniformLocation(_pid, "DZ"), sqrt(2) / 2048.0);

	// Set texture input
	glUniform1i(glGetUniformLocation(_pid, "tex_in_height"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *tex_height);

	glUniform1i(glGetUniformLocation(_pid, "tex_in_snow"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *tex_snow);

	glUniform1i(glGetUniformLocation(_pid, "tex_in_pos"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, *tex_pos);

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Unbind
	glBindVertexArray(0);
	glUseProgram(0);
}

void Quad::drawDirt(GLuint* tex_height, GLuint* tex_water, GLuint* tex_sediment,
	GLuint* tex_flux_LR, GLuint* tex_flux_TB, GLuint* tex_velocity) {

	// Bind program & vertex array
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	_erosion_params->setup(_pid);

	glUniform1f(glGetUniformLocation(_pid, "DX"), 1.0 / 2048);
	glUniform1f(glGetUniformLocation(_pid, "DY"), 1.0 / 2048);
	glUniform1f(glGetUniformLocation(_pid, "DZ"), sqrt(2) / 2048.0);

	// Set texture input
	glUniform1i(glGetUniformLocation(_pid, "height_in"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *tex_height);

	glUniform1i(glGetUniformLocation(_pid, "water_in"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *tex_water);

	glUniform1i(glGetUniformLocation(_pid, "sediment_in"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, *tex_sediment);

	glUniform1i(glGetUniformLocation(_pid, "flux_LR_in"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, *tex_flux_LR);

	glUniform1i(glGetUniformLocation(_pid, "flux_TB_in"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, *tex_flux_TB);

	glUniform1i(glGetUniformLocation(_pid, "velocity_in"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, *tex_velocity);

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Unbind
	glBindVertexArray(0);
	glUseProgram(0);

}

void Quad::drawGrass(GLuint* tex_height, GLuint* tex_grass, int action){
	// Bind program & vertex array
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	_grass_params->setup(_pid);

	glUniform1i(glGetUniformLocation(_pid, "action"), action);

	glUniform1f(glGetUniformLocation(_pid, "DX"), 1.0 / 2048);
	glUniform1f(glGetUniformLocation(_pid, "DY"), 1.0 / 2048);
	glUniform1f(glGetUniformLocation(_pid, "DZ"), sqrt(2) / 2048.0);

	// Set texture input
	glUniform1i(glGetUniformLocation(_pid, "tex_in_height"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *tex_height);

	glUniform1i(glGetUniformLocation(_pid, "tex_in_grass"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *tex_grass);

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Unbind
	glBindVertexArray(0);
	glUseProgram(0);
}

void Quad::drawTexture(GLuint* texture, float amplitude, float offset){

	// Bind program & vertex array
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	glUniform1f(glGetUniformLocation(_pid, "amplitude"), amplitude);
	glUniform1f(glGetUniformLocation(_pid, "offset"), offset);

	// Set texture input
	glUniform1i(glGetUniformLocation(_pid, "tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture);

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Unbind
	glBindVertexArray(0);
	glUseProgram(0);
}

