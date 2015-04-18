#pragma once
#include "icg_common.h"


class Water {
private:
	static const int grid_dim_ = 2048;

protected:
	GLuint _vao;          ///< vertex array object
	GLuint _vbo_position; ///< memory buffer for positions
	GLuint _vbo_index;    ///< memory buffer for indice
	GLuint _vbo;
	GLuint _pid;          ///< GLSL shader program ID
	GLuint _height_map;    ///< HeightMap Texture ID
	GLuint _color;        ///< Color Texture ID
	GLuint _num_indices;  ///< number of vertices to render

public:

	int get_vertex_index(int i, int j, int grid_dim) {
		return i + j * grid_dim;
	}

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

	void setHeightMap(GLuint* height_map) {
		///--- Texture coordinates
		const GLfloat vtexcoord[] = { /*V1*/ 0.0f, 0.0f,
			/*V2*/ 1.0f, 0.0f,
			/*V3*/ 0.0f, 1.0f,
			/*V4*/ 1.0f, 1.0f };

		///--- Buffer
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

		///--- Attribute
		GLuint vtexcoord_id = glGetAttribLocation(_pid, "vtexcoord");
		glEnableVertexAttribArray(vtexcoord_id);
		glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

		// Pass texture to instance
		this->_height_map = *height_map;

		GLuint tex_id = glGetUniformLocation(_pid, "tex");
		glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
	}

	void setColor(GLuint* color){
		// Pass texture to instance
		this->_color = *color;
		GLuint tex_id = glGetUniformLocation(_pid, "color1D");
		glUniform1i(tex_id, 1 /*GL_TEXTURE1*/);
	}

	void draw(const mat4& model, const mat4& view, const mat4& projection){
		glUseProgram(_pid);
		glBindVertexArray(_vao);

		// Texture uniforms
		glUniform1i(glGetUniformLocation(_pid, "heightmap"), 0 /*GL_TEXTURE0*/);
		glUniform1i(glGetUniformLocation(_pid, "color1D"), 1 /*GL_TEXTURE1*/);

		// Bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _height_map);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _color);

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
