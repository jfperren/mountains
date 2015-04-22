#pragma once
#include "icg_common.h"

class Water {

private:
	int grid_size = 2048;

protected:
	GLuint _vao;          ///< vertex array object
	GLuint _vbo_position; ///< memory buffer for positions
	GLuint _vbo_index;    ///< memory buffer for indice
	GLuint _vbo;
	GLuint _tex_main;
	GLuint _tex_mirror;
	GLuint _tex_height;
	GLuint _pid;          ///< GLSL shader program ID
	GLuint _num_indices;  ///< number of vertices to render

public:

	float height;
	float transparency;
	float color[3];
	float depth_alpha_factor;
	float depth_color_factor;
	float reflection_factor;

	void init(){
		///--- Compile the shaders
		_pid = opengp::load_shaders("water/Water_vshader.glsl", "water/Water_fshader.glsl");
		if (!_pid) exit(EXIT_FAILURE);
		glUseProgram(_pid);

		height = 0;
		transparency = 0.9;
		depth_alpha_factor = 0;
		depth_color_factor = 0;
		reflection_factor = 0.25;

		color[0] = 0.1f;
		color[1] = 0.3f;
		color[2] = 0.6f;

		///--- Vertex one vertex Array
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		///--- Vertex coordinates
		{
			const GLfloat vpoint[] = {	
				/*V1*/ -1.0f, -1.0f,
				/*V2*/ +1.0f, -1.0f,
				/*V3*/ -1.0f, +1.0f,
				/*V4*/ +1.0f, +1.0f
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

		///--- Texture coordinates
		{
			const GLfloat vtexcoord[] = { 
				/*V1*/ 0.0f, 0.0f,
				/*V2*/ 1.0f, 0.0f,
				/*V3*/ 0.0f, 1.0f,
				/*V4*/ 1.0f, 1.0f };

			///--- Buffer
			glGenBuffers(1, &_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

			///--- Attribute
			GLuint vertex_tex_coord_id = glGetAttribLocation(_pid, "vertex_tex_coord");
			glEnableVertexAttribArray(vertex_tex_coord_id);
			glVertexAttribPointer(vertex_tex_coord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
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
	}

	void cleanup(){
		glDeleteBuffers(1, &_vbo_position);
		glDeleteBuffers(1, &_vbo_index);
		glDeleteVertexArrays(1, &_vao);
		glDeleteProgram(_pid);
	}

	void setMainTexture(GLuint tex_main){
		// Pass texture to instance
		_tex_main = tex_main;
	}

	void setMirrorTexture(GLuint tex_mirror){
		// Pass texture to instance
		_tex_mirror = tex_mirror;
	}

	void setHeightTexture(GLuint tex_height) {
		// Pass texture to instance
		_tex_height = tex_height;
	}

	void draw(const mat4& model, const mat4& view, const mat4& projection){
		glUseProgram(_pid);
		glBindVertexArray(_vao);

		// Send Uniforms
		glUniform1f(glGetUniformLocation(_pid, "water_height"), height);
		glUniform1f(glGetUniformLocation(_pid, "water_transparency"), transparency);
		glUniform3f(glGetUniformLocation(_pid, "water_color"), color[0], color[1], color[2]);
		glUniform1f(glGetUniformLocation(_pid, "water_depth_alpha_factor"), depth_alpha_factor);
		glUniform1f(glGetUniformLocation(_pid, "water_depth_color_factor"), depth_color_factor);
		glUniform1f(glGetUniformLocation(_pid, "water_reflection_factor"), reflection_factor);

		///--- Texture uniforms
		GLuint tex_id = glGetUniformLocation(_pid, "tex_main");
		glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
		GLuint tex_mirror_id = glGetUniformLocation(_pid, "tex_mirror");
		glUniform1i(tex_mirror_id, 1 /*GL_TEXTURE1*/);
		GLuint tex_height_id = glGetUniformLocation(_pid, "tex_height");
		glUniform1i(tex_height_id, 2 /*GL_TEXTURE1*/);

		///--- Bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _tex_main);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _tex_mirror);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _tex_height);

		// Setup MVP
		mat4 MVP = projection*view*model;
		GLuint MVP_id = glGetUniformLocation(_pid, "mvp");
		glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

		// Draw
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glUseProgram(0);
	}
};