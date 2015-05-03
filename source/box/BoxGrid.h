#pragma once
#include "icg_common.h"

// Vertices [Left/Right] [Back/Front] [Bottom/Top]
#define LFB -1, -1, -1
#define LFT -1, +1, -1
#define LBB -1, -1, +1
#define LBT -1, +1, +1
#define RFB +1, -1, -1
#define RFT +1, +1, -1
#define RBB +1, -1, +1
#define RBT +1, +1, +1

class BoxGrid {

private:
	mat4 model = mat4::Identity();
	WaterParams* _water_params;
	GridParams* _grid_params;

protected:
	GLuint _vao;          ///< vertex array object
	GLuint _vbo_pos;
	GLuint _vbo_tex;
	GLuint _tex_height;
	GLuint _pid;          ///< GLSL shader program ID
	GLuint _num_indices;  ///< number of vertices to render

public:
	void init(GridParams* grid_params, WaterParams* water_params){
		///--- Compile the shaders
		_pid = opengp::load_shaders("box/box_vshader.glsl", "box/box_fshader.glsl");
		if (!_pid) exit(EXIT_FAILURE);
		glUseProgram(_pid);

		_water_params = water_params;
		_grid_params = grid_params;

		///--- Vertex one vertex Array
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		///--- Vertex coordinates
		{
			const GLfloat faces[] = {
				LFB, LBB, LFT, LBT, // Left
				RFB, RBB, RFT, RBT, // Right 
				LFB, RFB, LFT, RFT, // Front
				LBB, RBB, LBT, RBT, // Back
				LFB, RFB, LBB, RBB  // Bottom
			};

			// position buffer
			glGenBuffers(1, &_vbo_pos);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);

			// position shader attribute
			GLuint vertex_pos_id = glGetAttribLocation(_pid, "vertex_pos");
			glEnableVertexAttribArray(vertex_pos_id);
			glVertexAttribPointer(vertex_pos_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
		}

		///--- Texture coordinates
		{
			const GLfloat vtexcoord[] = {
				/*V1*/ 0.0f, 0.0f,
				/*V2*/ 1.0f, 0.0f,
				/*V3*/ 0.0f, 1.0f,
				/*V4*/ 1.0f, 1.0f };

			///--- Buffer
			glGenBuffers(1, &_vbo_tex);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_tex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

			///--- Attribute
			GLuint vertex_tex_coord_id = glGetAttribLocation(_pid, "vertex_tex_coord");
			glEnableVertexAttribArray(vertex_tex_coord_id);
			glVertexAttribPointer(vertex_tex_coord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
		}

		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		///--- to avoid the current object being polluted
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void cleanup(){
		glDeleteBuffers(1, &_vbo_tex);
		glDeleteBuffers(1, &_vbo_pos);
		glDeleteVertexArrays(1, &_vao);
		glDeleteProgram(_pid);
	}

	void setHeightTexture(GLuint tex_height) {
		// Pass texture to instance
		_tex_height = tex_height;
	}

	void draw(const mat4& view, const mat4& projection){
		glUseProgram(_pid);
		glBindVertexArray(_vao);

		// Send Uniforms
		_water_params->setup(_pid);
		_grid_params->setup(_pid);

		///--- Texture uniforms
		GLuint tex_height_id = glGetUniformLocation(_pid, "tex_height");
		glUniform1i(tex_height_id, 15 /*GL_TEXTURE15*/);

		glActiveTexture(GL_TEXTURE15);
		glBindTexture(GL_TEXTURE_2D, _tex_height);

		// Setup MVP
		mat4 MVP = projection*view*model;
		GLuint MVP_id = glGetUniformLocation(_pid, "mvp");
		glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

		// Draw
		for (GLuint i = 0; i < 20; i += 4){
			glDrawArrays(GL_TRIANGLE_STRIP, i, 4);
		}

		glBindVertexArray(0);
		glUseProgram(0);
	}
};