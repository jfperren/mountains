#pragma once
#include "icg_common.h"

class FullScreenQuad{
protected:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID 
    GLuint _vbo; ///< memory buffer
    GLuint _tex; ///< Texture ID

	// Dimensions compared to the unit vector
	int _width = 3;
	int _height = 3;

	vec2 gradients[3][3];

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
    
    void draw(){
		GLuint texture;

		

        glUseProgram(_pid);
        glBindVertexArray(_vao);
			// Pass _width and _height
			glUniform1f(glGetUniformLocation(_pid, "noise_width"), _width);
			glUniform1f(glGetUniformLocation(_pid, "noise_height"), _height);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);        
        glBindVertexArray(0);        
        glUseProgram(0);
    }


};
