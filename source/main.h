#pragma once

#include "icg_common.h"
#include "camera/camera.h"
#include "grid/Grid.h"
#include "noise/NoiseQuad.h"
#include "noise/NoiseGenerator.h"
#include "framebuffer/framebuffer.h"
#include "framebuffer/depthbuffer.h"
#include "water/Water.h"
#include "sky/sky.h"
#include <iostream>
#include <fstream>
#include <string>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include "box/BoxGrid.h"
#include "constants.h"
#include "io/IO.h"

#ifdef WITH_ANTTWEAKBAR
#include <AntTweakBar.h>
TwBar *bar;
#endif

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;

// --- Window --- //

WindowParams window_params{ WIDTH, HEIGHT };

// --- Grid --- //

GridParams grid_params;

// --- Scene Objects --- // 

Grid grid;
Water water;
BoxGrid box;
Sky sky;

// --- FrameBuffers --- // 

FB fbw(WIDTH, HEIGHT);
Depthbuffer fb_water_depth(WIDTH, HEIGHT);

// --- Textures --- //

GLuint tex_height;
GLuint tex_mirror;
GLuint tex_water_depth;

/* TODO:
At this time, there are 4 harded textures. We might want to refactor the code and
work with a single array of textures in the future.
*/

// Texture for underwater rock
GLuint tex_texture0;

// Texture for grass
GLuint tex_texture1;

// Texture for rock
GLuint tex_texture2;

// Texture for sand
GLuint tex_texture3;

// Texture for snow
GLuint tex_texture4;

// --- Variables for AntTweakBar ---

NoiseParams noise_values;
FractalParams fractal_values;
WaterParams water_params;
LightParams light_params;

// --- I/O ---

string g_file_name = "";
string g_file_name_load = "";

Camera camera(&window_params);

// --- Callbacks ---
void GLFWCALL OnWindowSize(int width, int height);
void GLFWCALL OnChar(int glfwChar, int glfwAction);
void GLFWCALL OnKey(int glfwKey, int glfwAction);
void GLFWCALL OnMouseWheel(int pos);
void GLFWCALL OnMousePos(int mouseX, int mouseY);
void GLFWCALL OnMouseButton(int glfwButton, int glfwAction);

void initParams() {

	// --- Window ---
	window_params.width = WIDTH;
	window_params.height = HEIGHT;

	// --- Grid ---
	grid_params.chunk_resolution = 100;
	grid_params.length_in_chunks = 2;
	grid_params.width_in_chunks = 2;
	
	// --- Noise ---
	noise_values.type = PERLIN_NOISE;
	noise_values.effect = NO_EFFECT;
	noise_values.height = 1;
	noise_values.width = 1;
	noise_values.offset = 0.0f;
	noise_values.amplitude = 1.0f;
	noise_values.seed = glfwGetTime();
	noise_values.seed -= floor(noise_values.seed);

	// --- Fractal ---
	fractal_values.fractal_type = FBM;
	fractal_values.fractal_effect = NO_EFFECT;
	fractal_values.amplitude = 1.0f;
	fractal_values.offset = 0.0f;
	fractal_values.H = 0.8f;
	fractal_values.lacunarity = 2;
	fractal_values.octaves = 6;
	fractal_values.enable = true;

	// --- Water ---
	water_params.height = 0;
	water_params.color = vec3(0.4f, 0.55f, 0.6f);
	water_params.depth_alpha_factor = 1.4f;
	water_params.depth_color_factor = 0.05f;
	water_params.transparency = 0.4f;
	water_params.reflection_factor = 0.25f;

	// --- Light ---
	light_params.Ia = vec3(0.7f, 0.7f, 0.7f);
	light_params.Id = vec3(0.3f, 0.3f, 0.3f);
	light_params.position = vec3(2.0f, 2.0f, 2.0f);
}

void initSceneObjects() {
	box.init(&grid_params, &water_params);
	grid.init(&grid_params, &light_params);
	grid.setMainTexture(tex_texture0, tex_texture1, tex_texture2, tex_texture3, tex_texture4);

	tex_mirror = fbw.init_texture();
	fbw.init(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);

	tex_water_depth = fb_water_depth.init_texture();
	//fb_water_depth.init(GL_FLOAT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
	fb_water_depth.init();

	water.init(&grid_params, &water_params);
	water.set_depth_texture(tex_water_depth);
	sky.init();
}

void initTextures() {

}
