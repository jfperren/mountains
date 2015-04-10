#pragma once
#include "icg_common.h"


class FullScreenQuad{
public:
	// Types of noise
	const static int NO_NOISE = -1;
	const static int RANDOM_NOISE = 0;
	const static int PERLIN_NOISE = 1;

protected:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID 
    GLuint _vbo; ///< memory buffer
    GLuint _tex; ///< Texture ID

public:
    void init(){
        
        ///--- Compile the shaders
        _pid = opengp::load_shaders("fullscreenquad/FullScreenQuad_vshader.glsl", "fullscreenquad/FullScreenQuad_fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);       
        glUseProgram(_pid);
        
        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
     
        ///--- Vertex coordinates
        {
            const GLfloat vpoint[] = { /*V1*/ -1.0f, -1.0f, 0.0f, 
                                       /*V2*/ +1.0f, -1.0f, 0.0f, 
                                       /*V3*/ -1.0f, +1.0f, 0.0f,
                                       /*V4*/ +1.0f, +1.0f, 0.0f };        
            ///--- Buffer
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);
        
            ///--- Attribute
            GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
            glEnableVertexAttribArray(vpoint_id);
            glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
		}
        
        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }
       
    void cleanup(){
        // TODO cleanup
    }
    
	void drawNoise(int noise_type, int width, int height, float offset, float amplitude, GLuint* texture = nullptr){

		// Bind program & vertex array
        glUseProgram(_pid);
        glBindVertexArray(_vao);

		// Pass parameters 
		glUniform1f(glGetUniformLocation(_pid, "noise_width"), width);
		glUniform1f(glGetUniformLocation(_pid, "noise_height"), height);
		glUniform1f(glGetUniformLocation(_pid, "offset"), offset);
		glUniform1f(glGetUniformLocation(_pid, "amplitude"), amplitude);
		glUniform1i(glGetUniformLocation(_pid, "noise_type"), noise_type);

		// Create a seed for randomness
		glUniform1f(glGetUniformLocation(_pid, "seed"), 0.11);

		if (texture != nullptr) {
			// Tells shader there is a texture as input
			glUniform1i(glGetUniformLocation(_pid, "is_texture"), 1);

			// Set texture input
			glUniform1i(glGetUniformLocation(_pid, "tex"), 0 /*GL_TEXTURE0*/);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *texture);
		}


		// Draw
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    

		// Unbind
        glBindVertexArray(0);
        glUseProgram(0);
    }
};
