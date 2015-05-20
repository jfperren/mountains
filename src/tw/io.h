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
#include "../app/constants.h"

void writeFile(string file_name, NoiseParams *noise_params, WaterParams *water_params);
void loadFromFile(string file_name, NoiseParams *noise_params, WaterParams *water_params);