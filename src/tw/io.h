#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <ctime>

#include "icg_common.h"
#include "../app/constants.h"
#include "../camera/bezier.h"

void writeFile(string file_name, WindowParams* window_params,
	ThemeParams* theme_params,
	GridParams* grid_params,
	NoiseParams* noise_params,
	GrassParams* grass_params,
	SandParams* sand_params,
	SnowParams* snow_params,
	ErosionParams* erosion_params,
	TextureParams* texture_params,
	ShadingParams* shading_params,
	WaterParams* water_params,
	Bezier* bezier);
void loadFromFile(string file_name, WindowParams* window_params,
	ThemeParams* theme_params,
	GridParams* grid_params,
	NoiseParams* noise_params,
	GrassParams* grass_params,
	SandParams* sand_params,
	SnowParams* snow_params,
	ErosionParams* erosion_params,
	TextureParams* texture_params,
	ShadingParams* shading_params,
	WaterParams* water_params,
	Bezier* bezier);
bool save_screenshot(string file_name, int width, int height);
string get_unique_name();