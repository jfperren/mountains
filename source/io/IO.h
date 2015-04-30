#pragma once

#include "icg_common.h"
#include "../trackball/Trackball.h"
#include "../grid/Grid.h"
#include "../framebuffer/Framebuffer.h"
#include "../framebuffer/FramebufferWater.h"
#include "../noise/NoiseQuad.h"
#include "../noise/NoiseGenerator.h"
#include "../water/Water.h"
#include "../main.h"
#include <iostream>
#include <fstream>
#include <string>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include "../box/BoxGrid.h"
#include "../constants.h"

// Dumps noise and fractal info to file_name
void writeFile(string file_name, NoiseParams *noise_params, FractalParams *fractal_params, WaterParams *water_params) {
	ofstream myfile(file_name);

	if (myfile.is_open()) {

		/* Write header */
		myfile << IO_HEADER_STRING << endl;

		/* The order is not important */

		/* Noise */
		myfile << "noise_type " << noise_params->type << endl;
		myfile << "noise_width " << noise_params->width << endl;
		myfile << "noise_height " << noise_params->height << endl;
		myfile << "noise_offset " << noise_params->offset << endl;
		myfile << "noise_amplitude " << noise_params->amplitude << endl;
		myfile << "noise_seed " << noise_params->seed << endl;

		/* Fractal */
		if (fractal_params->enable) {
			myfile << "fractal_enable " << "true" << endl;
			myfile << "fractal_H " << fractal_params->H << endl;
			myfile << "fractal_lacunarity " << fractal_params->lacunarity << endl;
			myfile << "fractal_octaves " << fractal_params->octaves << endl;
			myfile << "fractal_offset " << fractal_params->offset << endl;
			myfile << "fractal_amplitude " << fractal_params->amplitude << endl;

		}
		else {
			myfile << "fractal_enable " << "false" << endl;
		}

		/* Water */
		myfile << "water_height " << water_params->height << endl;
		myfile << "water_transparency " << water_params->transparency << endl;
		myfile << "water_depth_alpha_factor " << water_params->depth_alpha_factor << endl;
		myfile << "water_depth_color_factor " << water_params->depth_color_factor << endl;
		myfile << "water_color (" << water_params->color[0] << "," << water_params->color[1] << "," << water_params->color[2] << ")" << endl;


		myfile.close();
		std::cout << "[Info] Data saved to " << file_name << endl;
	}
	else {
		std::cout << "[Error] Could not save data: the file " << file_name << " could not be opened." << endl;
	}
}

void loadFromFile(string file_name, NoiseParams *noise_params, FractalParams *fractal_params, WaterParams *water_params) {
	string line;
	ifstream myfile(file_name);
	if (myfile.is_open())
	{

		int line_no = 0;

		while (getline(myfile, line))
		{
			line_no++;

			if (line_no == 1) {
				if (line.compare(IO_HEADER_STRING)) {
					// the first line doesn't match the header -> illegal format
					std::cout << "Error: Illegal header. Aborting load." << endl;
					return;
				}
			}
			string str = line;

			// construct a stream from the string
			stringstream strstr(str);

			// use stream iterators to copy the stream to the vector as whitespace separated strings
			istream_iterator<string> it(strstr);
			istream_iterator<string> end;
			vector<string> results(it, end);

			// send the vector to stdout.
			ostream_iterator<string> oit(std::cout);
			copy(results.begin(), results.end(), oit);

			/* Load fractal */
			if (!results[0].compare("fractal_enable")) {
				if (!results[1].compare("true")) {
					fractal_params->enable = true;
				}
				else {
				}
			}
			else if (!results[0].compare("fractal_H")) {
				fractal_params->H = ::atof(results[1].c_str());
			}
			else if (!results[0].compare("fractal_lacunarity")) {
				fractal_params->lacunarity = ::atoi(results[1].c_str());
			}
			else if (!results[0].compare("fractal_octaves")) {
				fractal_params->octaves = ::atoi(results[1].c_str());
			}
			else if (!results[0].compare("fractal_offset")) {
				fractal_params->offset = ::atof(results[1].c_str());
			}
			else if (!results[0].compare("fractal_amplitude")) {
				fractal_params->amplitude = ::atof(results[1].c_str());
			}
			
			/* Load noise */
			else if (!results[0].compare("noise_type")) {
				int type = ::atoi(results[1].c_str());
				switch (type) {
				case 0: noise_params->type = COPY_TEXTURE;
					break;
				case 1: noise_params->type = NO_NOISE;
					break;
				case 2: noise_params->type = RANDOM_NOISE;
					break;
				case 3: noise_params->type = PERLIN_NOISE;
					break;
				case 4: noise_params->type = PERLIN_NOISE_ABS;
					break;
				default:
					std::cout << "[Error] Unkown NoiseType" << endl;
					break;
				}

			}
			else if (!results[0].compare("noise_width")) {
				noise_params->width = ::atoi(results[1].c_str());
			}
			else if (!results[0].compare("noise_height")) {
				noise_params->height = ::atoi(results[1].c_str());
			}
			else if (!results[0].compare("noise_offset")) {
				noise_params->offset = ::atof(results[1].c_str());
			}
			else if (!results[0].compare("noise_amplitude")) {
				noise_params->amplitude = ::atof(results[1].c_str());
			}
			else if (!results[0].compare("noise_seed")) {
				noise_params->seed = ::atof(results[1].c_str());
			}

			/* Load water */
			else if (!results[0].compare("water_height")) {
				water_params->height = ::atof(results[1].c_str());
			}
			else if (!results[0].compare("water_transparency")) {
				water_params->transparency = ::atof(results[1].c_str());
			}
			else if (!results[0].compare("water_depth_alpha_factor")) {
				water_params->depth_alpha_factor = ::atof(results[1].c_str());
			}
			else if (!results[0].compare("water_depth_color_factor")) {
				water_params->depth_color_factor = ::atof(results[1].c_str());
			}
			else if (!results[0].compare("water_color")) {
				/* TODO load color */
			}
		}

		/*myfile << "water_height " << water_params->height << endl;
		myfile << "water_transparency " << water_params->transparency << endl;
		myfile << "water_depth_alpha_factor " << water_params->depth_alpha_factor << endl;
		myfile << "water_depth_color_factor " << water_params->depth_color_factor << endl;
		myfile << "water_color (" << water_params->color[0] << "," << water_params->color[1] << "," << water_params->color[2] << endl;*/

		myfile.close();
		std::cout << "[Info] Data loaded from " << file_name << endl;
	}
	else {
		std::cout << "[Error] Could not load data: the file" << file_name << " could not be opened." << endl;
	}
}