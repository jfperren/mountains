#include "water.h"

void Water::init(GridParams* grid_params, WaterParams* water_params, LightParams* light_params){
	///--- Compile the shaders
	_pid = opengp::load_shaders("scene/water_vshader.glsl", "scene/water_fshader.glsl");
	if (!_pid) exit(EXIT_FAILURE);
	glUseProgram(_pid);

	_water_params = water_params;
	_grid_params = grid_params;
	_light_params = light_params;

	///--- Vertex one vertex Array
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	///--- Vertex coordinates
	{
		const GLfloat vpoint[] = {
			-1.0f, -1.0f,
			+1.0f, -1.0f,
			-1.0f, +1.0f,
			+1.0f, +1.0f
		};

		///--- Buffer
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

		///--- Attribute
		GLuint vertex_pos_id = glGetAttribLocation(_pid, "vertex_pos");
		glEnableVertexAttribArray(vertex_pos_id);
		glVertexAttribPointer(vertex_pos_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
	}

	///--- Load texture
	glGenTextures(1, &_tex_main);
	glBindTexture(GL_TEXTURE_2D, _tex_main);
	glfwLoadTexture2D("_floor/Floor_texture.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	///--- to avoid the current object being polluted
	glBindVertexArray(0);
	glUseProgram(0);
	check_error_gl();
}

void Water::cleanup(){
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
	glDeleteProgram(_pid);
}

void Water::set_main_texture(GLuint tex_main){
	// Pass texture to instance
	_tex_main = tex_main;
}

void Water::set_mirror_texture(GLuint tex_mirror){
	// Pass texture to instance
	_tex_mirror = tex_mirror;
}

void Water::set_height_texture(GLuint tex_height) {
	// Pass texture to instance
	_tex_height = tex_height;
}

void Water::set_depth_texture(GLuint tex_depth) {
	_tex_depth = tex_depth;
}

void Water::set_normal_map(GLuint tex_normal_map) {
	_tex_normal_map = tex_normal_map;
}

void Water::draw(const mat4& view, const mat4& projection){
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	_water_params->setup(_pid);
	_grid_params->setup(_pid);

	glUniform1f(glGetUniformLocation(_pid, "near"), NEAR);
	glUniform1f(glGetUniformLocation(_pid, "far"), FAR);

	///--- Texture uniforms
	GLuint tex_id = glGetUniformLocation(_pid, "tex_main");
	glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
	GLuint tex_mirror_id = glGetUniformLocation(_pid, "tex_mirror");
	glUniform1i(tex_mirror_id, 1 /*GL_TEXTURE1*/);
	GLuint tex_height_id = glGetUniformLocation(_pid, "tex_height");
	glUniform1i(tex_height_id, 2 /*GL_TEXTURE2*/);
	GLuint tex_depth_id = glGetUniformLocation(_pid, "tex_depth");
	glUniform1i(tex_depth_id, 3 /*GL_TEXTURE3*/);
	GLuint tex_normal_map_id = glGetUniformLocation(_pid, "tex_normal_map");
	glUniform1i(tex_normal_map_id, 4 /*GL_TEXTURE4*/);

	///--- Bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _tex_main);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _tex_mirror);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _tex_height);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _tex_depth);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, _tex_normal_map);

	// Setup MVP
	mat4 MVP = projection*view*model;
	GLuint MVP_id = glGetUniformLocation(_pid, "mvp");
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glUseProgram(0);
}