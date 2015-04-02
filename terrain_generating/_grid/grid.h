#pragma once
#include "icg_common.h"

class Grid{

private:
	static const int grid_dim_ = 100;

protected:
    GLuint _vao;          ///< vertex array object
    GLuint _vbo_position; ///< memory buffer for positions
    GLuint _vbo_index;    ///< memory buffer for indice
    GLuint _pid;          ///< GLSL shader program ID
    GLuint _tex;          ///< Texture ID
    GLuint _num_indices;  ///< number of vertices to render
    
public:    

	int get_vertex_index(int i, int j, int grid_dim) {
		return i + j * grid_dim;
	}

    void init(){
        // Compile the shaders
        _pid = opengp::load_shaders("grid_vshader.glsl", "grid_fshader.glsl");
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
				float x = float(i) * (2.0f / (grid_dim - 1)) - 1;
				float y = float(j) * (2.0f / (grid_dim - 1)) - 1;
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
        
        // Load texture
        glGenTextures(1, &_tex);
        glBindTexture(GL_TEXTURE_2D, _tex);
        glfwLoadTexture2D("grid_texture.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                
        // Texture uniforms
        GLuint tex_id = glGetUniformLocation(_pid, "tex");
        glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
        
        // to avoid the current object being polluted
        glBindVertexArray(0);
    }
           
    void cleanup(){
        glDeleteBuffers(1, &_vbo_position);
        glDeleteBuffers(1, &_vbo_index);
        glDeleteVertexArrays(1, &_vao);
        glDeleteProgram(_pid);
        glDeleteTextures(1, &_tex);
    }
    
    void draw(const mat4& model, const mat4& view, const mat4& projection, float time){
        glUseProgram(_pid);
        glBindVertexArray(_vao);
        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _tex);

        // Setup MVP
        mat4 MVP = projection*view*model;
        GLuint MVP_id = glGetUniformLocation(_pid, "mvp");
        glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

        // Pass the current time stamp to the shader.
        glUniform1f(glGetUniformLocation(_pid, "time"), time);

        // Draw
        glDrawElements(GL_TRIANGLES, _num_indices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);        
        glUseProgram(0);
    }
};
