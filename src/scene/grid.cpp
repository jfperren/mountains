#include "grid.h"

using namespace std;


	int Grid::get_vertex_index(int i, int j) {
		return i + j * _grid_params->resolution * _grid_params->length;
	}

	void Grid::init(AppParams* app_params, GLuint* tex_height, GLuint* tex_snow){

		_tex_height = tex_height;
		_tex_snow = tex_snow;

		_noise_params = app_params->noise_params;
		_grid_params = app_params->grid_params;
		_texture_params = app_params->texture_params;
		_shading_params = app_params->shading_params;
		_snow_params = app_params->snow_params;

		TEX_PATHS[0] = "textures/terrains/mountain/tex_grass.tga";
		TEX_PATHS[1] = "textures/terrains/mountain/tex_sand.tga";
		TEX_PATHS[2] = "textures/terrains/mountain/tex_rock.tga";
		TEX_PATHS[3] = "textures/terrains/mountain/tex_snow.tga";
		TEX_PATHS[4] = "textures/terrains/mountain/tex_rock_underwater.tga";

		TEX_NAMES[0] = "tex_grass";
		TEX_NAMES[1] = "tex_sand";
		TEX_NAMES[2] = "tex_rock";
		TEX_NAMES[3] = "tex_snow";
		TEX_NAMES[4] = "tex_rock_underwater";

		// Compile the shaders
		_pid = opengp::load_shaders("scene/grid_vshader.glsl", "scene/grid_fshader.glsl");
		if (!_pid) exit(EXIT_FAILURE);
		glUseProgram(_pid);

		// Vertex one vertex Array
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// Vertex coordinates and indices
		{
			std::vector<GLfloat> vertices;
			std::vector<GLuint> indices;

			// Grid dimension
			int grid_length = _grid_params->resolution * _grid_params->length;
			int grid_width = _grid_params->resolution * _grid_params->width;

			cout << grid_length << endl;
			cout << grid_width << endl;

			// Put vertex positions
			for (int i = 0; i < grid_length; i++) {
				for (int j = 0; j < grid_width; j++) {
					float x = -0.5 + float(i) / (grid_length - 1);
					float y = -0.5 + float(j) / (grid_width - 1);
					vertices.push_back(x);
					vertices.push_back(y);
				}
			}

			for (int i = 0; i < grid_length - 1; i++) {
				for (int j = 0; j < grid_width - 1; j++) {
					// triangle (i,j), (i+1, j), (i, j+1)
					indices.push_back(get_vertex_index(i, j));
					indices.push_back(get_vertex_index(i + 1, j));
					indices.push_back(get_vertex_index(i, j + 1));

					indices.push_back(get_vertex_index(i + 1, j));
					indices.push_back(get_vertex_index(i, j + 1));
					indices.push_back(get_vertex_index(i + 1, j + 1));
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
			GLuint vertex_pos_id = glGetAttribLocation(_pid, "vertex_pos");
			glEnableVertexAttribArray(vertex_pos_id);
			glVertexAttribPointer(vertex_pos_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
		}

		// --- Create Textures ---

		{
			for (int i = 0; i < TEXTURES_COUNT; i++){

				GLuint tex;
				const char* full_name = (TEX_PATH + TEX_NAMES[i] + TEX_EXT).data();
				glGenTextures(1, &tex);
				glBindTexture(GL_TEXTURE_2D, tex);
				glfwLoadTexture2D(TEX_PATHS[i], 0);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				GLuint tex_snow_id = glGetUniformLocation(_pid, TEX_NAMES[i].data());
				glUniform1i(tex_snow_id, 11 + i);
				glActiveTexture(GL_TEXTURE11 + i);
				glBindTexture(GL_TEXTURE_2D, tex);

				_texs.push_back(tex);
			}
		}

		// to avoid the current object being polluted
		glBindVertexArray(0);
		check_error_gl();
	}

	void Grid::cleanup(){
		glDeleteBuffers(1, &_vbo_position);
		glDeleteBuffers(1, &_vbo_index);
		glDeleteVertexArrays(1, &_vao);
		glDeleteProgram(_pid);
	}

	void Grid::draw(const mat4& view, const mat4& projection, bool only_reflect){
		glUseProgram(_pid);
		glBindVertexArray(_vao);

		_grid_params->setup(_pid);
		_texture_params->setup(_pid);
		_shading_params->setup(_pid);
		_snow_params->setup(_pid);

		glUniform1f(glGetUniformLocation(_pid, "DX"), 1.0 / _noise_params->resolution);
		glUniform1f(glGetUniformLocation(_pid, "DY"), 1.0 / _noise_params->resolution);

		if (only_reflect){
			glUniform1i(glGetUniformLocation(_pid, "only_reflect"), 1);
		}
		else {
			glUniform1i(glGetUniformLocation(_pid, "only_reflect"), 0);
		}

		// Bind textures
		glUniform1i(glGetUniformLocation(_pid, "tex_height"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *_tex_height);

		glUniform1i(glGetUniformLocation(_pid, "tex_snow"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, *_tex_snow);

		/*for (int i = 0; i < TEXTURES_COUNT; i++){

			GLuint tex_snow_id = glGetUniformLocation(_pid, TEX_NAMES[i].data());
			glUniform1i(tex_snow_id, 1 + i);
			glActiveTexture(GL_TEXTURE1 + i);
			glBindTexture(GL_TEXTURE_2D, _texs[i]);
		}*/

		// Setup MVP
		mat4 MVP = projection*view*model;
		GLuint MVP_id = glGetUniformLocation(_pid, "mvp");
		glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

		// Draw
		glDrawElements(GL_TRIANGLES, _num_indices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
