#include "icg_common.h"
#include "trackball/Trackball.h"
#include "grid/Grid.h"
#include "framebuffer/Framebuffer.h"
#include "framebuffer/FramebufferWater.h"
#include "noise/NoiseQuad.h"
#include "noise/NoiseGenerator.h"
#include "water/Water.h"
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

#ifdef WITH_ANTTWEAKBAR
#include <AntTweakBar.h>
TwBar *bar;
#endif

using namespace std;

// --- Window --- //

int WIDTH = 800;
int HEIGHT = 600;

// --- View --- //

vec3 cam_up;
vec3 cam_pos;
vec3 cam_dir;
vec3 old_cam_pos;
mat4 projection_matrix;
mat4 view_matrix;
mat4 grid_model_matrix;
mat4 water_model_matrix;

Trackball trackball;
float zoom_start_y;

// --- Scene Objects --- // 

Grid grid;
Water water;
BoxGrid box;

// --- FrameBuffers --- // 

FramebufferWater fbw(WIDTH, HEIGHT);

// --- Textures --- //

GLuint tex_height;
GLuint tex_mirror;
GLuint color;

// --- Variables for AntTweakBar ---

NoiseParams noise_values;
FractalParams fractal_values;
WaterParams water_params;

// --- I/O ---

string g_file_name = "";
string g_file_name_load = "";

void initParams() {
	
	// --- Noise ---
	noise_values.noise_type = PERLIN_NOISE;
	noise_values.noise_effect = NO_EFFECT;
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

	// --- Water
	water_params.height = 0;
	water_params.color = vec3(0.1f, 0.5f, 0.9f);
	water_params.depth_alpha_factor = 0.0f;
	water_params.depth_color_factor = 0.0f;
	water_params.transparency = 0.9f;
	water_params.reflection_factor = 0.25f;
}

void initSceneObjects() {
	box.init();
	grid.init();
	grid.setMainTexture(color);
	tex_mirror = fbw.init();
	water.init(&water_params);
}

void initViewMatrices() {
	projection_matrix = Eigen::perspective(45.0f, WIDTH / (float)HEIGHT, 0.1f, 10.0f);

	grid_model_matrix = mat4::Identity();
	water_model_matrix = mat4::Identity();

	cam_pos = vec3(0.0f, 2.0f, 4.0f);
	cam_dir = vec3(0.0f, 0.0f, 0.0f);
	cam_up = vec3(0.0f, 1.0f, 0.0f);

	view_matrix = lookAt(cam_pos, cam_dir, cam_up);
}

void initTextures() {
	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);
	glfwLoadTexture2D("textures/texture1D.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
