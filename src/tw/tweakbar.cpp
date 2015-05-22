#include "tweakbar.h"

#ifdef WITH_ANTTWEAKBAR
TwBar *bar;
#endif

string g_file_name = "";
string g_file_name_load = "";

void initAntTwBar(AppParams* app_params) {

	WindowParams* window_params		= app_params->window_params;
	GridParams* grid_params			= app_params->grid_params;
	NoiseParams* noise_params		= app_params->noise_params;
	SnowParams* snow_params			= app_params->snow_params;
	ErosionParams* erosion_params	= app_params->erosion_params;
	TextureParams* texture_params	= app_params->texture_params;
	ShadingParams* shading_params	= app_params->shading_params;
	WaterParams* water_params		= app_params->water_params;

	TwInit(TW_OPENGL_CORE, NULL);
	// Needed to work with dynamic strings
	TwCopyStdStringToClientFunc(CopyStdStringToClient);
	TwWindowSize(window_params->width, window_params->height);
	bar = TwNewBar("Settings");
	
	// Grid

	TwAddVarCB(bar, "grid_enable", TW_TYPE_BOOL8, setBoolParamCallback, getBoolParamCallback, &grid_params->enable, " group=Grid");
	TwAddVarCB(bar, "grid_length", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &grid_params->length, " group=Grid step=1");
	TwAddVarCB(bar, "grid_width", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &grid_params->width, " group=Grid step=1");
	TwAddVarCB(bar, "grid_resolution", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &grid_params->resolution, " group=Grid step=1");
	
	// Noise

	TwAddVarCB(bar, "noise_resolution", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &noise_params->resolution, " group=Noise step=1");

	TwEnumVal noise_type_array[] = { { NO_NOISE, "NO_NOISE" }, { RANDOM_NOISE, "RANDOM_NOISE" }, { PERLIN_NOISE, "PERLIN_NOISE" }, { WORLEY_NOISE, "WORLEY_NOISE" } };
	TwType noise_type_type = TwDefineEnum("NoiseType", noise_type_array, 4);
	TwAddVarCB(bar, "noise_ntype", noise_type_type, setIntParamCallback, getIntParamCallback, &noise_params->noise_type, " group=Noise ");

	TwEnumVal fractal_type_array[] = { { FBM, "FBM" }, { MULTIFRACTAL, "MULTIFRACTAL" } };
	TwType fractal_type = TwDefineEnum("FractalType", fractal_type_array, 2);
	TwAddVarCB(bar, "noise_ftype", fractal_type, setIntParamCallback, getIntParamCallback, &noise_params->fractal_type, " group=Noise ");

	TwEnumVal noise_effect_array[] = { { NO_EFFECT, "NO_EFFECT" }, { ABSOLUTE_VALUE, "ABSOLUTE_VALUE" }, { CLAMP_EXTREMAS, "CLAMP_EXTREMAS" }, { DISCRETIZE, "DISCRETIZE" } };
	TwType noise_effect_type = TwDefineEnum("NoiseEffect", noise_effect_array, 4);
	TwAddVarCB(bar, "noise_neffect", noise_effect_type, setIntParamCallback, getIntParamCallback, &noise_params->noise_effect, " group=Noise ");

	TwEnumVal fractal_effect_array[] = { { NO_EFFECT, "NO_EFFECT" }, { ABSOLUTE_VALUE, "ABSOLUTE_VALUE" }, { CLAMP_EXTREMAS, "CLAMP_EXTREMAS" }, { DISCRETIZE, "DISCRETIZE" } };
	TwType fractal_effect_type = TwDefineEnum("FractalEffect", fractal_effect_array, 4);
	TwAddVarCB(bar, "noise_feffect", fractal_effect_type, setIntParamCallback, getIntParamCallback, &noise_params->fractal_effect, " group=Noise ");

	TwAddVarCB(bar, "noise_width", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &noise_params->width, " group=Noise step=1");
	TwAddVarCB(bar, "noise_height", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &noise_params->height, " group=Noise step=1");
	TwAddVarCB(bar, "noise_offset", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_params->offset, " group=Noise step=0.1");
	TwAddVarCB(bar, "noise_amplitude", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_params->amplitude, " group=Noise step=0.1");

	TwAddVarCB(bar, "noise_H", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_params->H, " group=Noise step=0.1");
	TwAddVarCB(bar, "noise_lacunarity", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &noise_params->lacunarity, " group=Noise step=1 min=2");
	TwAddVarCB(bar, "noise_octaves", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &noise_params->octaves, " group=Noise step=1 min=1");

	TwAddVarCB(bar, "noise_seed", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_params->seed, " group=Noise step=0.001 min=0 max=1");

	// Snow

	TwAddVarCB(bar, "snow_enable", TW_TYPE_BOOL8, setBoolParamCallback, getBoolParamCallback, &snow_params->enable, " group=Snow");
	TwAddVarCB(bar, "snow_amount", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &snow_params->amount, " group=Snow step=0.01 min=0");
	TwAddVarCB(bar, "snow_max_amount", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &snow_params->max_amount, " group=Snow step=0.01 min=0");
	TwAddVarCB(bar, "snow_max_slope", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &snow_params->max_slope, " group=Snow step=0.01 min=0.01");
	TwAddVarCB(bar, "snow_min_height", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &snow_params->min_height, " group=Snow step=0.01");
	TwAddVarCB(bar, "snow_threshold", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &snow_params->threshold, " group=Snow step=0.001");
	TwAddVarCB(bar, "snow_slide_time", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &snow_params->slide_time, " group=Snow step=1 min=0");
	TwAddVarCB(bar, "snow_melt_time", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &snow_params->melt_time, " group=Snow step=1 min=0");
	TwAddVarCB(bar, "snow_smooth_time", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &snow_params->smooth_time, " group=Snow step=1 min=0");

	// Erosion

	TwAddVarCB(bar, "deposition_rate", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &erosion_params->deposition_rate, " group=Erosion step=0.001 min=0 max=1");
	TwAddVarCB(bar, "erosion_rate", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &erosion_params->erosion_rate, " group=Erosion step=0.001 min=0 max=1");
	TwAddVarCB(bar, "evaporation_rate", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &erosion_params->evaporation_rate, " group=Erosion step=0.001 min=0 max=1");
	TwAddVarCB(bar, "rain_rate", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &erosion_params->rain_rate, " group=Erosion step=0.001 min=0 max=1");

	TwAddVarCB(bar, "sediment_capacity", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &erosion_params->sediment_capacity, " group=Erosion step=0.001 min=0 max=1");
	TwAddVarCB(bar, "direction_intertia", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &erosion_params->direction_inertia, " group=Erosion step=0.001  min=0 max=1");

	TwAddVarCB(bar, "iterations", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &erosion_params->iterations, " group=Erosion step=5 min=0");

	// Water 

	TwAddVarCB(bar, "water_enable", TW_TYPE_BOOL8, setBoolParamCallback, getBoolParamCallback, &water_params->enable, " group=Water");
	TwAddVarCB(bar, "water_height", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->height, " group=Water step=0.1");
	TwAddVarCB(bar, "water_alpha", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->transparency, " group=Water step=0.1 min=0, max=1");
	TwAddVarCB(bar, "water_depth_alpha_factor", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->depth_alpha_factor, " group=Water step=0.05 min=0");
	TwAddVarCB(bar, "water_depth_color_factor", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->depth_color_factor, " group=Water step=0.05 min=0");
	TwAddVarCB(bar, "water_reflection_factor", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->reflection_factor, " group=Water step=0.05 min=0 max=1");
	TwAddVarCB(bar, "water_color_R", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->color[0], " group=Water step=0.05 min=0 max=1");
	TwAddVarCB(bar, "water_color_G", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->color[1], " group=Water step=0.05 min=0 max=1");
	TwAddVarCB(bar, "water_color_B", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->color[2], " group=Water step=0.05 min=0 max=1");

	// Texture 

	TwEnumVal texture_type_array[] = { { NO_TEXTURE, "NO_TEXTURE" }, { SHADES, "SHADES" }, { TEXTURE, "TEXTURE" } };
	TwType texture_type_type = TwDefineEnum("TextureType", texture_type_array, 3);
	TwAddVarCB(bar, "texture_type", texture_type_type, setIntParamCallback, getIntParamCallback, &texture_params->texture_type, " group=Texture ");

	TwAddVarCB(bar, "sand_min_height", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &texture_params->sand_min_height, " group=Texture step=0.05");
	TwAddVarCB(bar, "sand_max_height", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &texture_params->sand_max_height, " group=Texture step=0.05");
	TwAddVarCB(bar, "grass_max_height", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &texture_params->grass_max_height, " group=Texture step=0.05");
	TwAddVarCB(bar, "sand_max_slope", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &texture_params->sand_max_slope, " group=Texture step=0.02");
	TwAddVarCB(bar, "grass_max_slope", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &texture_params->grass_max_slope, " group=Texture step=0.02");

	TwAddVarCB(bar, "grass_h_transition", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &texture_params->grass_h_transition, " group=Texture step=1");
	TwAddVarCB(bar, "grass_s_transition", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &texture_params->grass_s_transition, " group=Texture step=1");
	TwAddVarCB(bar, "sand_h_transition", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &texture_params->sand_h_transition, " group=Texture step=1");
	TwAddVarCB(bar, "sand_s_transition", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &texture_params->sand_s_transition, " group=Texture step=1");

	// Shading

	TwAddVarCB(bar, "shading_enable", TW_TYPE_BOOL8, setBoolParamCallback, getBoolParamCallback, &shading_params->enable, " group=Shading");
	TwAddVarCB(bar, "shading_light_X", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &shading_params->light_pos[0], " group=Shading step=0.1");
	TwAddVarCB(bar, "shading_light_Y", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &shading_params->light_pos[1], " group=Shading step=0.1");
	TwAddVarCB(bar, "shading_light_Z", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &shading_params->light_pos[2], " group=Shading step=0.1");
	TwAddVarCB(bar, "shading_Ia_R", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &shading_params->Ia[0], " group=Shading step=0.05 min=0 max=1");
	TwAddVarCB(bar, "shading_Ia_G", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &shading_params->Ia[1], " group=Shading step=0.05 min=0 max=1");
	TwAddVarCB(bar, "shading_Ia_B", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &shading_params->Ia[2], " group=Shading step=0.05 min=0 max=1");
	TwAddVarCB(bar, "shading_Id_R", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &shading_params->Id[0], " group=Shading step=0.05 min=0 max=1");
	TwAddVarCB(bar, "shading_Id_G", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &shading_params->Id[1], " group=Shading step=0.05 min=0 max=1");
	TwAddVarCB(bar, "shading_Id_B", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &shading_params->Id[2], " group=Shading step=0.05 min=0 max=1");

	
	// I/O 

	TwAddVarRW(bar, "save_file_name", TW_TYPE_STDSTRING, &g_file_name, " group='I/O' label='file_name (optional)' ");
	TwAddButton(bar, "Save", SaveCB, NULL, " group='I/O' ");
	TwAddVarRW(bar, "load_file_name", TW_TYPE_STDSTRING, &g_file_name_load, " group='I/O' ");
	TwAddButton(bar, "Load", LoadCB, NULL, " group='I/O' ");
}

// --- Value Callbacks --- //

void TW_CALL setIntParamCallback(const void* value, void* clientData) {
	*((int*)clientData) = *((int*)value);
	compute_height_map();
}

void TW_CALL setFloatParamCallback(const void* value, void* clientData) {
	*((float*)clientData) = *((float*)value);
	compute_height_map();
}

void TW_CALL setBoolParamCallback(const void* value, void* clientData) {
	*((bool*)clientData) = *((bool*)value);
	compute_height_map();
}

void TW_CALL getIntParamCallback(void* value, void* clientData) {
	*((int*)value) = *((int*)clientData);
}

void TW_CALL getFloatParamCallback(void* value, void* clientData) {
	*((float*)value) = *((float*)clientData);
}

void TW_CALL getBoolParamCallback(void* value, void* clientData) {
	*((bool*)value) = *((bool*)clientData);
}

// --- Save Functions --- // 

// Function called by AntTweakBar to copy the content of a std::string handled
// by the AntTweakBar library to a std::string handled by your application
void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
{
	destinationClientString = sourceLibraryString;
}

void TW_CALL SaveCB(void * /*clientData*/)
{
	if (!g_file_name.compare("")) {
		std::stringstream sstm;
		sstm << "mountain-" << glfwGetTime() << ".terrain";
		g_file_name = sstm.str();
		g_file_name_load = g_file_name; // optional
	}

	//writeFile(g_file_name, &noise_params, &fractal_values);
}

void TW_CALL LoadCB(void * /*clientData*/)
{
	if (!g_file_name_load.compare("")) {
		// Empty name
		cout << "Error: Cannot load from empty name" << endl;
	}
	else {
		//loadFromFile(g_file_name_load, &noise_params, &fractal_values);
	}

	// Update scene with the changes
	compute_height_map();
}