#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <algorithm>

#include "icg_common.h"
#include "camera/camera.h"
#include "grid/grid.h"
#include "noise/noise_quad.h"
#include "noise/noise_generator.h"
#include "framebuffer/framebuffer.h"
#include "framebuffer/depthbuffer.h"
#include "water/water.h"
#include "sky/sky.h"
#include "tw/tweakbar.h"
#include "box/box.h"
#include "constants.h"
#include "io/io.h"

using namespace std;

// --- Callbacks ---

void GLFWCALL OnWindowSize(int width, int height);
void GLFWCALL OnChar(int glfwChar, int glfwAction);
void GLFWCALL OnKey(int glfwKey, int glfwAction);
void GLFWCALL OnMouseWheel(int pos);
void GLFWCALL OnMousePos(int mouseX, int mouseY);
void GLFWCALL OnMouseButton(int glfwButton, int glfwAction);

void compute_height_map();
void resize_callback(int width, int height);

void initParams();
void initSceneObjects();
void initTextures();