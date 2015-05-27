#include "io.h"

/* Writes parameters to file_name */
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
	WaterParams* water_params) {
	ofstream myfile(file_name);

	if (myfile.is_open()) {

		/* Write header */
		myfile << IO_HEADER_STRING << endl << IO_HEADER_CREDITS << endl;

		/* The order is only relevant for clarity and readability */

		/* General */
		myfile << "\n# --- General --- #\n" << endl;

		// Must be NO_THEME (0) if we want the settings to be editable from the TwBar
		myfile << "theme_params.theme_type " << theme_params->theme_type << endl;

		/* Window */
		myfile << "\n# --- Window --- #\n" << endl;
		myfile << "window_params.width " << window_params->width << endl;
		myfile << "window_params.height " << window_params->height << endl;

		/* Grid */
		myfile << "\n# --- Grid --- #\n" << endl;
		myfile << "grid_params.enable " << grid_params->enable << endl;
		myfile << "grid_params.resolution " << grid_params->resolution << endl;
		myfile << "grid_params.length " << grid_params->length << endl;
		myfile << "grid_params.width " << grid_params->width << endl;

		/* Noise */
		myfile << "\n# --- Noise --- #\n" << endl;
		myfile << "noise_params.noise_type " << noise_params->noise_type << endl;
		myfile << "noise_params.fractal_type " << noise_params->fractal_type << endl;
		myfile << "noise_params.noise_effect " << noise_params->noise_effect << endl;
		myfile << "noise_params.fractal_effect " << noise_params->fractal_effect << endl;
		myfile << "noise_params.resolution " << noise_params->resolution << endl;
		myfile << "noise_params.height " << noise_params->height << endl;
		myfile << "noise_params.width " << noise_params->width << endl;
		myfile << "noise_params.offset " << noise_params->offset << endl;
		myfile << "noise_params.amplitude " << noise_params->amplitude << endl;
		myfile << "noise_params.H " << noise_params->H << endl;
		myfile << "noise_params.lacunarity " << noise_params->lacunarity << endl;
		myfile << "noise_params.octaves " << noise_params->octaves << endl;
		myfile << "noise_params.seed " << noise_params->seed << endl;

		/* Grass */
		myfile << "\n# --- Grass --- #\n" << endl;
		myfile << "grass_params.enable " << grass_params->enable << endl;
		myfile << "grass_params.min_height " << grass_params->min_height << endl;
		myfile << "grass_params.max_height " << grass_params->max_height << endl;
		myfile << "grass_params.max_slope " << grass_params->max_slope << endl;
		myfile << "grass_params.min_angle " << grass_params->min_angle << endl;
		myfile << "grass_params.time_grow " << grass_params->time_grow << endl;
		myfile << "grass_params.time_smooth " << grass_params->time_smooth << endl;

		/* Sand */
		myfile << "\n# --- Sand --- #\n" << endl;
		myfile << "sand_params.enable " << sand_params->enable << endl;
		myfile << "sand_params.amount " << sand_params->amount << endl;
		myfile << "sand_params.max_amount " << sand_params->max_amount << endl;
		myfile << "sand_params.min_height " << sand_params->min_height << endl;
		myfile << "sand_params.max_height " << sand_params->max_height << endl;
		myfile << "sand_params.max_slope " << sand_params->max_slope << endl;
		myfile << "sand_params.threshold " << sand_params->threshold << endl;
		myfile << "sand_params.slide_time " << sand_params->slide_time << endl;
		myfile << "sand_params.melt_time " << sand_params->melt_time << endl;
		myfile << "sand_params.smooth_time " << sand_params->smooth_time << endl;

		/* Snow */
		myfile << "\n# --- Snow --- #\n" << endl;
		myfile << "snow_params.enable " << snow_params->enable << endl;
		myfile << "snow_params.amount " << snow_params->amount << endl;
		myfile << "snow_params.max_amount " << snow_params->max_amount << endl;
		myfile << "snow_params.min_height " << snow_params->min_height << endl;
		myfile << "snow_params.max_slope " << snow_params->max_slope << endl;
		myfile << "snow_params.threshold " << snow_params->threshold << endl;
		myfile << "snow_params.slide_time " << snow_params->slide_time << endl;
		myfile << "snow_params.melt_time " << snow_params->melt_time << endl;
		myfile << "snow_params.smooth_time " << snow_params->smooth_time << endl;

		/* Erosion */
		myfile << "\n# --- Erosion --- #\n" << endl;
		myfile << "erosion_params.deposition_rate " << erosion_params->deposition_rate << endl;
		myfile << "erosion_params.erosion_rate " << erosion_params->erosion_rate << endl;
		myfile << "erosion_params.rain_rate " << erosion_params->rain_rate << endl;
		myfile << "erosion_params.evaporation_rate " << erosion_params->evaporation_rate << endl;
		myfile << "erosion_params.sediment_capacity " << erosion_params->sediment_capacity << endl;
		myfile << "erosion_params.direction_inertia " << erosion_params->direction_inertia << endl;
		myfile << "erosion_params.iterations " << erosion_params->iterations << endl;

		/* Water */
		myfile << "\n# --- Water --- #\n" << endl;
		myfile << "water_params.enable " << water_params->enable << endl;
		myfile << "water_params.height " << water_params->height << endl;
		myfile << "water_params.color " << water_params->color[0] << ' ' << water_params->color[1] << ' ' << water_params->color[2] << endl;
		myfile << "water_params.depth_alpha_factor " << water_params->depth_alpha_factor << endl;
		myfile << "water_params.depth_color_factor " << water_params->depth_color_factor << endl;
		myfile << "water_params.transparency " << water_params->transparency << endl;
		myfile << "water_params.reflection_factor " << water_params->reflection_factor << endl;
		myfile << "water_params.waves_speed " << water_params->waves_speed << endl;
		myfile << "water_params.waves_tile_factor " << water_params->waves_tile_factor << endl;
		myfile << "water_params.waves_amplitude " << water_params->waves_amplitude << endl;

		/* Shading */
		myfile << "\n# --- Shading --- #\n" << endl;
		myfile << "shading_params.enable_phong " << shading_params->enable_phong << endl;
		myfile << "shading_params.enable_shadow " << shading_params->enable_shadow << endl;
		myfile << "shading_params.shadow_intensity " << shading_params->shadow_intensity << endl;
		myfile << "shading_params.Ia " << shading_params->Ia[0] << ' ' << shading_params->Ia[1] << ' ' << shading_params->Ia[2] << endl;
		myfile << "shading_params.Id " << shading_params->Id[0] << ' ' << shading_params->Id[1] << ' ' << shading_params->Id[2] << endl;
		myfile << "shading_params.light_pos " << shading_params->light_pos[0] << ' ' << shading_params->light_pos[1] << ' ' << shading_params->light_pos[2] << endl;
		myfile << "shading_params.far " << shading_params->far << endl;
		myfile << "shading_params.near " << shading_params->near << endl;

		/* Texture */
		myfile << "\n# --- Texture --- #\n" << endl;
		myfile << "texture_params.texture_type " << texture_params->texture_type << endl;
		myfile << "texture_params.sand_min_height " << texture_params->sand_min_height << endl;
		myfile << "texture_params.sand_max_height " << texture_params->sand_max_height << endl;
		myfile << "texture_params.grass_max_height " << texture_params->grass_max_height << endl;
		myfile << "texture_params.sand_max_slope " << texture_params->sand_max_slope << endl;
		myfile << "texture_params.grass_max_slope " << texture_params->grass_max_slope << endl;
		myfile << "texture_params.grass_h_transition " << texture_params->grass_h_transition << endl;
		myfile << "texture_params.grass_s_transition " << texture_params->grass_s_transition << endl;
		myfile << "texture_params.sand_h_transition " << texture_params->sand_h_transition << endl;
		myfile << "texture_params.sand_s_transition " << texture_params->sand_s_transition << endl;

		/* End of data */
		myfile << "\n# --- END OF DATA --- #" << endl;

		myfile.close();
		std::cout << "[Info] Data saved to " << file_name << endl;
	}
	else {
		std::cout << "[Error] Could not save data: the file " << file_name << " could not be opened." << endl;
	}
}

/* Loads parameters from file_name */
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
	WaterParams* water_params) {

	string line;
	ifstream myfile(file_name);

	if (myfile.is_open())
	{

		int line_no = 0;

		while (getline(myfile, line))
		{
			line_no++;

			// Assert we are looking at a terrain file and not at some pdf or other kind of files
			if (line_no == 1) {
				if (line.compare(IO_HEADER_STRING)) {
					std::cout << "[Error] Bad header. Load aborted." << endl;
					return;
				}
			}

			// Ignore empty lines or comments
			if (line == "" || line[0] == '#') continue;

			string str = line;

			// Construct a stream from the string
			stringstream strstr(str);

			// Use stream iterators to copy the stream to the vector as whitespace separated strings
			istream_iterator<string> it(strstr);
			istream_iterator<string> end;
			vector<string> results(it, end);

			string variable = results[0];


			/* General */
			if (!variable.compare("theme_params.theme_type")) {
				int type = ::atoi(results[1].c_str());
				// TODO fix, doesn't work
				theme_params->theme_type = ThemeType(type);
			}
			
			/* Window */
			else if (!variable.compare("window_params.width")) {
				// TODO save window properties?
				window_params->width = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("window_params.height")) {
				window_params->height = ::atoi(results[1].c_str());
			} 
			
			/* Grid */
			else if (!variable.compare("grid_params.enable")) {
				grid_params->enable = (results[1].c_str());
			}
			else if (!variable.compare("grid_params.resolution")) {
				grid_params->resolution = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("grid_params.length")) {
				grid_params->length = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("grid_params.width")) {
				grid_params->width = ::atoi(results[1].c_str());
			}
			
			/* Noise */
			else if (!variable.compare("noise_params.noise_type")) {
				// TODO fix
				noise_params->noise_type = NoiseType(::atoi(results[1].c_str()));
			}
			else if (!variable.compare("noise_params.fractal_type")) {
				// TODO fix
				noise_params->fractal_type = FractalType(::atoi(results[1].c_str()));
			}
			else if (!variable.compare("noise_params.noise_effect")) {
				// TODO fix
				noise_params->noise_effect = Effect(::atoi(results[1].c_str()));
			}
			else if (!variable.compare("noise_params.fractal_effect")) {
				// TODO fix
				noise_params->fractal_effect = Effect(::atoi(results[1].c_str()));
			}
			else if (!variable.compare("noise_params.resolution")) {
				noise_params->resolution = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("noise_params.height")) {
				noise_params->height = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("noise_params.width")) {
				noise_params->width = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("noise_params.offset")) {
				noise_params->offset = ::atof(results[1].c_str());
			}
			else if (!variable.compare("noise_params.amplitude")) {
				noise_params->amplitude = ::atof(results[1].c_str());
			}
			else if (!variable.compare("noise_params.H")) {
				noise_params->H = ::atof(results[1].c_str());
			}
			else if (!variable.compare("noise_params.lacunarity	")) {
				noise_params->lacunarity = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("noise_params.octaves")) {
				noise_params->octaves = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("noise_params.seed")) {
				noise_params->seed = ::atof(results[1].c_str());
			}

			/* Grass*/
			else if (!variable.compare("grass_params.enable")) {
				grass_params->enable = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("grass_params.min_height")) {
				grass_params->min_height = ::atof(results[1].c_str());
			}
			else if (!variable.compare("grass_params.max_height")) {
				grass_params->max_height = ::atof(results[1].c_str());
			}
			else if (!variable.compare("grass_params.max_slope")) {
				grass_params->max_slope = ::atof(results[1].c_str());
			}
			else if (!variable.compare("grass_params.min_angle")) {
				grass_params->min_angle = ::atof(results[1].c_str());
			}
			else if (!variable.compare("grass_params.time_grow")) {
				grass_params->time_grow = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("grass_params.time_smooth")) {
				grass_params->time_smooth = ::atoi(results[1].c_str());
			}

			/* Sand */
			else if (!variable.compare("sand_params.enable")) {
				sand_params->enable = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("sand_params.amount")) {
				sand_params->amount = ::atof(results[1].c_str());
			}
			else if (!variable.compare("sand_params.max_amount")) {
				sand_params->max_amount = ::atof(results[1].c_str());
			}
			else if (!variable.compare("sand_params.min_height")) {
				sand_params->min_height = ::atof(results[1].c_str());
			}
			else if (!variable.compare("sand_params.max_height")) {
				sand_params->max_height = ::atof(results[1].c_str());
			}
			else if (!variable.compare("sand_params.max_slope")) {
				sand_params->max_slope = ::atof(results[1].c_str());
			}
			else if (!variable.compare("sand_params.threshold")) {
				sand_params->threshold = ::atof(results[1].c_str());
			}
			else if (!variable.compare("sand_params.slide_time")) {
				sand_params->slide_time = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("sand_params.melt_time")) {
				sand_params->melt_time = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("sand_params.smooth_time")) {
				sand_params->smooth_time = ::atoi(results[1].c_str());
			}

			/* Snow */
			else if (!variable.compare("snow_params.enable")) {
				snow_params->enable = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("snow_params.amount")) {
				snow_params->amount = ::atof(results[1].c_str());
			}
			else if (!variable.compare("snow_params.max_amount")) {
				snow_params->max_amount = ::atof(results[1].c_str());
			}
			else if (!variable.compare("snow_params.min_height")) {
				snow_params->min_height = ::atof(results[1].c_str());
			}
			else if (!variable.compare("snow_params.max_slope")) {
				snow_params->max_slope = ::atof(results[1].c_str());
			}
			else if (!variable.compare("snow_params.threshold")) {
				snow_params->threshold = ::atof(results[1].c_str());
			}
			else if (!variable.compare("snow_params.slide_time")) {
				snow_params->slide_time = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("snow_params.melt_time")) {
				snow_params->melt_time = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("snow_params.smooth_time")) {
				snow_params->smooth_time = ::atoi(results[1].c_str());
			}

			/* Erosion */
			else if (!variable.compare("erosion_params.deposition_rate")) {
				erosion_params->deposition_rate = ::atof(results[1].c_str());
			}
			else if (!variable.compare("erosion_params.erosion_rate")) {
				erosion_params->erosion_rate = ::atof(results[1].c_str());
			}
			else if (!variable.compare("erosion_params.rain_rate")) {
				erosion_params->rain_rate = ::atof(results[1].c_str());
			}
			else if (!variable.compare("erosion_params.evaporation_rate")) {
				erosion_params->evaporation_rate = ::atof(results[1].c_str());
			}
			else if (!variable.compare("erosion_params.sediment_capacity")) {
				erosion_params->sediment_capacity = ::atof(results[1].c_str());
			}
			else if (!variable.compare("erosion_params.direction_inertia")) {
				erosion_params->direction_inertia = ::atof(results[1].c_str());
			}
			else if (!variable.compare("erosion_params.iterations")) {
				erosion_params->iterations = ::atoi(results[1].c_str());
			}

			/* Water */
			else if (!variable.compare("water_params.enable")) {
				water_params->enable = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("water_params.height")) {
				water_params->height = ::atof(results[1].c_str());
			}
			else if (!variable.compare("water_params.color")) {
				water_params->color = vec3(::atof(results[1].c_str()), ::atof(results[2].c_str()), ::atof(results[3].c_str()));
			}
			else if (!variable.compare("water_params.depth_alpha_factor")) {
				water_params->depth_alpha_factor = ::atof(results[1].c_str());
			}
			else if (!variable.compare("water_params.depth_color_factor")) {
				water_params->depth_color_factor = ::atof(results[1].c_str());
			}
			else if (!variable.compare("water_params.transparency")) {
				water_params->transparency = ::atof(results[1].c_str());
			}
			else if (!variable.compare("water_params.reflection_factor")) {
				water_params->reflection_factor = ::atof(results[1].c_str());
			}
			else if (!variable.compare("water_params.waves_speed")) {
				water_params->waves_speed = ::atof(results[1].c_str());
			}
			else if (!variable.compare("water_params.waves_tile_factor")) {
				water_params->waves_tile_factor = ::atof(results[1].c_str());
			}
			else if (!variable.compare("water_params.waves_amplitude")) {
				water_params->waves_amplitude = ::atof(results[1].c_str());
			}

			/* Shading */
			else if (!variable.compare("shading_params.enable_phong")) {
				shading_params->enable_phong = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("shading_params.enable_shadow")) {
				shading_params->enable_shadow = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("shading_params.shadow_intensity")) {
				shading_params->shadow_intensity = ::atof(results[1].c_str());
			}
			else if (!variable.compare("shading_params.Ia")) {
				shading_params->Ia = vec3(::atof(results[1].c_str()), ::atof(results[2].c_str()), ::atof(results[3].c_str()));
			}
			else if (!variable.compare("shading_params.Id")) {
				shading_params->Id = vec3(::atof(results[1].c_str()), ::atof(results[2].c_str()), ::atof(results[3].c_str()));
			}
			else if (!variable.compare("shading_params.light_pos")) {
				shading_params->light_pos = vec3(::atof(results[1].c_str()), ::atof(results[2].c_str()), ::atof(results[3].c_str()));
			}
			else if (!variable.compare("shading_params.far")) {
				shading_params->far = ::atof(results[1].c_str());
			}
			else if (!variable.compare("shading_params.near")) {
				shading_params->near = ::atof(results[1].c_str());
			}

			/* Texture */
			else if (!variable.compare("texture_params.texture_type")) {
				// TODO fix
				texture_params->texture_type = TextureType(::atoi(results[1].c_str()));
			}
			else if (!variable.compare("texture_params.sand_min_height")) {
				texture_params->sand_min_height = ::atof(results[1].c_str());
			}
			else if (!variable.compare("texture_params.sand_max_height")) {
				texture_params->sand_max_height = ::atof(results[1].c_str());
			}
			else if (!variable.compare("texture_params.grass_max_height")) {
				texture_params->grass_max_height = ::atof(results[1].c_str());
			}
			else if (!variable.compare("texture_params.sand_max_slope")) {
				texture_params->sand_max_slope = ::atof(results[1].c_str());
			}
			else if (!variable.compare("texture_params.grass_max_slope")) {
				texture_params->grass_max_slope = ::atof(results[1].c_str());
			}
			else if (!variable.compare("texture_params.grass_h_transition")) {
				texture_params->grass_h_transition = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("texture_params.grass_s_transition")) {
				texture_params->grass_s_transition = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("texture_params.sand_h_transition")) {
				texture_params->sand_h_transition = ::atoi(results[1].c_str());
			}
			else if (!variable.compare("texture_params.sand_s_transition")) {
				texture_params->sand_s_transition = ::atoi(results[1].c_str());
			}


			else {
				cout << "[Warning] No match (l." << line_no << "): the variable was <" << results[0] << ">" << endl;
			}
			/*else if (!results[0].compare("fractal_H")) {
				//fractal_params->H = ::atof(results[1].c_str());
			}*/

		}

		//myfile.close();
		std::cout << "[Info] Data loaded from " << file_name << endl;
	}
	else {
		std::cout << "[Error] Could not load data: the file" << file_name << " could not be opened." << endl;
	}
}

bool save_screenshot(string filename, int w, int h)
{

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	int nSize = w*h * 3;

	// 3 channels per Pixel
	char* dataBuffer = (char*)malloc(nSize*sizeof(char));

	if (!dataBuffer) return false;

	glReadPixels((GLint)0, (GLint)0,
		(GLint)w, (GLint)h,
		GL_BGR, GL_UNSIGNED_BYTE, dataBuffer);

	FILE *filePtr = fopen(filename.c_str(), "wb");
	if (!filePtr) return false;


	unsigned char TGAheader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char header[6] = { w % 256, w / 256,
		h % 256, h / 256,
		24, 0 };
	// Write headers
	fwrite(TGAheader, sizeof(unsigned char), 12, filePtr);
	fwrite(header, sizeof(unsigned char), 6, filePtr);

	// Write image data
	fwrite(dataBuffer, sizeof(GLubyte), nSize, filePtr);

	fclose(filePtr);
	delete[] dataBuffer;

	return true;
}

string get_unique_name() {
	std::stringstream ss;

	time_t t = time(0);
	struct tm * now = localtime(&t);

	ss << (now->tm_year + 1900) << '-'
		<< (now->tm_mon + 1) << '-'
		<< now->tm_mday << '-'
		<< now->tm_hour << '-'
		<< now->tm_min << '-'
		<< now->tm_sec;

	return ss.str();
}