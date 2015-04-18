#pragma once
#include "icg_common.h"

class Water {

protected:
	GLuint _vao;          ///< vertex array object
	GLuint _vbo_position; ///< memory buffer for positions
	GLuint _vbo_index;    ///< memory buffer for indice
	GLuint _vbo;
	GLuint _pid;          ///< GLSL shader program ID
	GLuint _num_indices;  ///< number of vertices to render

public:
	float height = 0.5;
	float alpha = 0.5;

	void init(){
		// Compile the shaders
		_pid = opengp::load_shaders("water/Water_vshader.glsl", "water/Water_fshader.glsl");
		if (!_pid) exit(EXIT_FAILURE);
		glUseProgram(_pid);

		// Vertex one vertex Array
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// Vertex coordinates and indices
		{
			std::vector<GLfloat> vertices;
			std::vector<GLuint> indices;

			vertices.push_back(-1);
			vertices.push_back(-1);

			vertices.push_back(-1);
			vertices.push_back(1);

			vertices.push_back(1);
			vertices.push_back(-1);

			vertices.push_back(1);
			vertices.push_back(1);

			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(2);

			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(3);

			_num_indices = indices.size();

			// position buffer
			glGenBuffers(1, &_vbo_position);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_position);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

			// vertex indices
			glGenBuffers(1, &_vbo_index);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

			// position shader attribute
			GLuint loc_position = glGetAttribLocation(_pid, "position");
			glEnableVertexAttribArray(loc_position);
			glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
		}

		// to avoid the current object being polluted
		glBindVertexArray(0);
	}

	void cleanup(){
		glDeleteBuffers(1, &_vbo_position);
		glDeleteBuffers(1, &_vbo_index);
		glDeleteVertexArrays(1, &_vao);
		glDeleteProgram(_pid);
	}


	void draw(const mat4& model, const mat4& view, const mat4& projection){
		glUseProgram(_pid);
		glBindVertexArray(_vao);

		// Send Uniforms
		glUniform1f(glGetUniformLocation(_pid, "height"), height);
		glUniform1f(glGetUniformLocation(_pid, "alpha"), alpha);

		// Setup MVP
		mat4 MVP = projection*view*model;
		GLuint MVP_id = glGetUniformLocation(_pid, "mvp");
		glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

		// Draw
		glDrawElements(GL_TRIANGLES, _num_indices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
};
