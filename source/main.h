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

Grid grid;
Water water;

int WIDTH = 800;
int HEIGHT = 600;

mat4 projection_matrix;
mat4 view_matrix;

mat4 grid_model_matrix;
mat4 water_model_matrix;

vec3 cam_up;
vec3 cam_pos;
vec3 cam_dir;

BoxGrid box;

vec3 old_cam_pos;

Trackball trackball;

FramebufferWater fbw(WIDTH, HEIGHT);

// Texture for noise
GLuint tex_height;
GLuint tex_mirror;

/* TODO:
At this time, there are 4 harded textures. We might want to refactor the code and
work with a single array of textures in the future.
*/
// Texture for grass
GLuint tex_texture1;

// Texture for rock
GLuint tex_texture2;

// Texture for sand
GLuint tex_texture3;

// Texture for snow
GLuint tex_texture4;

// Constants
const float kZoomFactor = 2;
const string kHeaderString = "HEADER: terrain data";

// Used to store old values in computation
mat4 old_trackball_matrix;
mat4 old_view_matrix;
float zoom_start_y;

// --- Variables for AntTweakBar ---

// --- Noise ---
NoiseValues noise_values;

// --- Fractal ---
FractalValues fractal_values;
bool fractal_enable;

// --- I/O ---
string g_file_name = "";
string g_file_name_load = "";