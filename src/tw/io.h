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

void writeFile(string file_name, NoiseParams *noise_params, WaterParams *water_params);
void loadFromFile(string file_name, NoiseParams *noise_params, WaterParams *water_params);
bool save_screenshot(string file_name, int width, int height);
string get_unique_name();