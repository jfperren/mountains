#pragma once
#include "icg_common.h"

class FullScreenQuad{

	// Types of noise
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
    
    void drawNoise(int width, int height){
		GLuint texture;

        glUseProgram(_pid);
        glBindVertexArray(_vao);
			// Pass _width and _height
			glUniform1f(glGetUniformLocation(_pid, "noise_width"), width);
			glUniform1f(glGetUniformLocation(_pid, "noise_height"), height);
			glUniform1i(glGetUniformLocation(_pid, "noise_type"), RANDOM_NOISE);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);        
        glBindVertexArray(0);
        glUseProgram(0);
    }
};
