#pragma once
#include "icg_common.h"
#include "../constants.h"

class NoiseQuad {

protected:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID 
    GLuint _vbo; ///< memory buffer
    GLuint _tex; ///< Texture ID

public:
    void init(){
        
        ///--- Compile the shaders
        _pid = opengp::load_shaders("noise/NoiseQuad_vshader.glsl", "noise/NoiseQuad_fshader.glsl");
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
    
	void drawNoise(NoiseParams noise_params, GLuint* in_texture = nullptr, int fractal_type = FBM){

		// Bind program & vertex array
        glUseProgram(_pid);
        glBindVertexArray(_vao);

		// Pass parameters to the shaders
		glUniform1f(glGetUniformLocation(_pid, "noise_width"), noise_params.width);
		glUniform1f(glGetUniformLocation(_pid, "noise_height"), noise_params.height);
		glUniform1f(glGetUniformLocation(_pid, "offset"), noise_params.offset);
		glUniform1f(glGetUniformLocation(_pid, "amplitude"), noise_params.amplitude);
		glUniform1i(glGetUniformLocation(_pid, "noise_type"), noise_params.noise_type);
		glUniform1i(glGetUniformLocation(_pid, "aggregation_type"), fractal_type);
		glUniform1f(glGetUniformLocation(_pid, "seed"), noise_params.seed);
		glUniform1i(glGetUniformLocation(_pid, "noise_effect"), noise_params.noise_effect);

		if (in_texture != nullptr) {
			// Tells shader there is a texture as input
			glUniform1i(glGetUniformLocation(_pid, "is_texture"), 1);

			// Set texture input
			glUniform1i(glGetUniformLocation(_pid, "tex"), 0 /*GL_TEXTURE0*/);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *in_texture);
		}

		// Draw
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    

		// Unbind
        glBindVertexArray(0);
        glUseProgram(0);
    }
};
