#pragma once
#include "icg_common.h"
#include "../constants.h"

using namespace std;

class Grid {

	string TEX_PATH = "textures/terrains/mountain/";
	string TEX_EXT = ".tga";

	const int GRASS = 0;
	const int SAND = 1;
	const int ROCK = 2;
	const int SNOW = 3;
	const int ROCK_UNDERWATER = 4;

	const int TEXTURES_COUNT = 5;

	string TEX_NAMES[5];


private:
	static const int grid_dim_ = 2048;
	LightParams* light_params;
	GridParams* _grid_params;
	mat4 model = mat4::Identity();

protected:
    GLuint _vao;          ///< vertex array object
    GLuint _vbo_position; ///< memory buffer for positions
    GLuint _vbo_index;    ///< memory buffer for indice
	GLuint _vbo;
    GLuint _pid;          ///< GLSL shader program ID
    GLuint _tex_height;    ///< HeightMap Texture ID

	GLuint _tex_main;

	vector<GLuint> _texs;     

    GLuint _num_indices;  ///< number of vertices to render
    
public:    

	int get_vertex_index(int i, int j) {
		return i + j * _grid_params->chunk_resolution * _grid_params->length_in_chunks;
	}

    void init(GridParams* grid_params, LightParams* light_params){

		TEX_NAMES[0] = "tex_grass";
		TEX_NAMES[1] = "tex_sand";
		TEX_NAMES[2] = "tex_rock";
		TEX_NAMES[3] = "tex_snow";
		TEX_NAMES[4] = "tex_rock_underwater";

		this->light_params = light_params;
		_grid_params = grid_params;

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
			int grid_length = _grid_params->chunk_resolution * _grid_params->length_in_chunks;
			int grid_width = _grid_params->chunk_resolution * _grid_params->width_in_chunks;

			// Put vertex positions
			for (int i = 0; i < grid_length ; i++) {
				for (int j = 0; j < grid_width; j++) {
					float x = 2 * float(i) * (_grid_params->length_in_chunks / (grid_length - 1.0f)) - _grid_params->length_in_chunks;
					float y = 2 * float(j) * (_grid_params->width_in_chunks / (grid_width - 1.0f)) - _grid_params->width_in_chunks;
					vertices.push_back(x);
					vertices.push_back(y);
				}
			}
	
			for (int i = 0; i < grid_length - 1; i++) {
				for (int j = 0; j < grid_width - 1; j++) {
					// triangle (i,j), (i+1, j), (i, j+1)
					indices.push_back(get_vertex_index(i, j));
					indices.push_back(get_vertex_index(i+1, j));
					indices.push_back(get_vertex_index(i, j+1));
				
					indices.push_back(get_vertex_index(i+1, j));
					indices.push_back(get_vertex_index(i, j+1));
					indices.push_back(get_vertex_index(i+1, j+1));
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

		{
			///--- Texture coordinates
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
			GLuint vtexcoord_id = glGetAttribLocation(_pid, "vtexcoord");
			glEnableVertexAttribArray(vtexcoord_id);
			glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
		}

		// --- Create Textures ---

		{
			for (int i = 0; i < TEXTURES_COUNT; i++){

				GLuint tex;
				string full_name = TEX_PATH + TEX_NAMES[i] + TEX_EXT;
					
				glGenTextures(1, &tex);
				glBindTexture(GL_TEXTURE_2D, tex);
				glfwLoadTexture2D(full_name.c_str, 0);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				GLuint tex_snow_id = glGetUniformLocation(_pid, &TEX_NAMES[i].c_str);
				glUniform1i(tex_snow_id, i);

				_texs.push_back(tex);
			}
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

	void setHeightTexture(GLuint tex_height) {
		// Pass texture to instance
		_tex_height = tex_height;
	}


    void draw(const mat4& view, const mat4& projection, bool only_reflect=false){
        glUseProgram(_pid);
        glBindVertexArray(_vao);

		light_params->setup(_pid);
		_grid_params->setup(_pid);

		// Texture uniforms
		glUniform1i(glGetUniformLocation(_pid, "tex_height"), 10 /*GL_TEXTURE10*/);
		glUniform1i(glGetUniformLocation(_pid, "tex_texture1"), 11 /*GL_TEXTURE11*/);
		glUniform1i(glGetUniformLocation(_pid, "tex_texture2"), 12 /*GL_TEXTURE12*/);
		glUniform1i(glGetUniformLocation(_pid, "tex_texture3"), 13 /*GL_TEXTURE13*/);
		glUniform1i(glGetUniformLocation(_pid, "tex_texture4"), 14 /*GL_TEXTURE14*/);

		if (only_reflect){
			glUniform1i(glGetUniformLocation(_pid, "only_reflect"), 1);
		} else {
			glUniform1i(glGetUniformLocation(_pid, "only_reflect"), 0);
		}

        // Bind textures
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, _tex_height);



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
