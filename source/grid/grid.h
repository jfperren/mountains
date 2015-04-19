#pragma once
#include "icg_common.h"

struct Light{
	vec3 Ia = vec3(0.7f, 0.7f, 0.7f);
	vec3 Id = vec3(0.3f, 0.3f, 0.3f);

	vec3 light_pos = vec3(2.0f, 2.0f, 2.0f);

	///--- Pass light properties to the shader
	void setup(GLuint _pid){
		glUseProgram(_pid);
		GLuint light_pos_id = glGetUniformLocation(_pid, "light_pos"); //Given in camera space
		GLuint Ia_id = glGetUniformLocation(_pid, "Ia");
		GLuint Id_id = glGetUniformLocation(_pid, "Id");
		glUniform3fv(light_pos_id, ONE, light_pos.data());
		glUniform3fv(Ia_id, ONE, Ia.data());
		glUniform3fv(Id_id, ONE, Id.data());
	}
};

class Grid : public Light{

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
        _pid = opengp::load_shaders("grid/Grid_vshader.glsl", "grid/Grid_fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);       
        glUseProgram(_pid);
        
        // Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
     
        // Vertex coordinates and indices
        {
        	std::vector<GLfloat> vertices;
            	std::vector<GLuint> indices;
            	
            	// Grid dimension
            	int grid_dim = 100;

			// Put vertex positions
			for (int i = 0; i < grid_dim; i++) {
				for (int j = 0; j < grid_dim; j++) {
					float x =  float(i) * (2.0f / (grid_dim - 1)) - 1;
					float y =  float(j) * (2.0f / (grid_dim - 1)) - 1;
					vertices.push_back(x);
					vertices.push_back(y);
				}
			}
	
			for (int i = 0; i < grid_dim - 1; i++) {
				for (int j = 0; j < grid_dim - 1; j++) {
					// triangle (i,j), (i+1, j), (i, j+1)
					indices.push_back(get_vertex_index(i, j, grid_dim));
					indices.push_back(get_vertex_index(i+1, j, grid_dim));
					indices.push_back(get_vertex_index(i, j+1, grid_dim));
				
					indices.push_back(get_vertex_index(i+1, j, grid_dim));
					indices.push_back(get_vertex_index(i, j+1, grid_dim));
					indices.push_back(get_vertex_index(i+1, j+1, grid_dim));
				}
			}

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

    void draw(const mat4& model, const mat4& view, const mat4& projection, bool only_reflect=false){
        glUseProgram(_pid);
        glBindVertexArray(_vao);

		Light::setup(_pid);

		// Texture uniforms
		glUniform1i(glGetUniformLocation(_pid, "heightmap"), 0 /*GL_TEXTURE0*/);
		glUniform1i(glGetUniformLocation(_pid, "color1D"), 1 /*GL_TEXTURE1*/);

		if (only_reflect){
			glUniform1i(glGetUniformLocation(_pid, "only_reflect"), 1);
		} else {
			glUniform1i(glGetUniformLocation(_pid, "only_reflect"), 0);
		}

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
