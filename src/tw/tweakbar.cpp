#include "tweakbar.h"

#ifdef WITH_ANTTWEAKBAR
TwBar *bar;
#endif

string g_file_name = "";
string g_file_name_load = "";

void initAntTwBar(GridParams* grid_params, WindowParams* window_params, NoiseParams* noise_params, WaterParams* water_params, TextureParams* texture_params) {

	TwInit(TW_OPENGL_CORE, NULL);
	// Needed to work with dynamic strings
	TwCopyStdStringToClientFunc(CopyStdStringToClient);
	TwWindowSize(window_params->width, window_params->height);
	bar = TwNewBar("Settings");

	// Grid

	TwAddVarCB(bar, "grid_length", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &grid_params->length, " group=Grid step=1");
	TwAddVarCB(bar, "grid_width", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &grid_params->width, " group=Grid step=1");
	TwAddVarCB(bar, "grid_resolution", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &grid_params->resolution, " group=Grid step=1");
	
	// Noise 

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

	// Water 

	TwAddVarCB(bar, "water_height", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->height, " group=Water step=0.1");
	TwAddVarCB(bar, "water_alpha", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->transparency, " group=Water step=0.1 min=0, max=1");
	TwAddVarCB(bar, "water_depth_alpha_factor", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->depth_alpha_factor, " group=Water step=0.05 min=0");
	TwAddVarCB(bar, "water_depth_color_factor", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->depth_color_factor, " group=Water step=0.05 min=0");
	TwAddVarCB(bar, "water_reflection_factor", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->reflection_factor, " group=Water step=0.05 min=0 max=1");
	TwAddVarCB(bar, "water_color_R", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->color[0], " group=Water step=0.05 min=0 max=1");
	TwAddVarCB(bar, "water_color_G", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->color[1], " group=Water step=0.05 min=0 max=1");
	TwAddVarCB(bar, "water_color_B", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params->color[2], " group=Water step=0.05 min=0 max=1");

	// Texture 

	TwAddVarCB(bar, "sand_min_height", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &texture_params->sand_min_height, " group=Texture step=0.05");
	TwAddVarCB(bar, "sand_max_height", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &texture_params->sand_max_height, " group=Texture step=0.05");
	TwAddVarCB(bar, "grass_max_height", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &texture_params->grass_max_height, " group=Texture step=0.05");
	TwAddVarCB(bar, "sand_max_slope", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &texture_params->sand_max_slope, " group=Texture step=0.02");
	TwAddVarCB(bar, "grass_max_slope", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &texture_params->grass_max_slope, " group=Texture step=0.02");

	TwAddVarCB(bar, "grass_h_transition", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &texture_params->grass_h_transition, " group=Texture step=1");
	TwAddVarCB(bar, "grass_s_transition", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &texture_params->grass_s_transition, " group=Texture step=1");
	TwAddVarCB(bar, "sand_h_transition", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &texture_params->sand_h_transition, " group=Texture step=1");
	TwAddVarCB(bar, "sand_s_transition", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &texture_params->sand_s_transition, " group=Texture step=1");

	// I/O 

	TwAddVarRW(bar, "save_file_name", TW_TYPE_STDSTRING, &g_file_name, " group='I/O' label='file_name (optional)' ");
	TwAddButton(bar, "Save", SaveCB, NULL, " group='I/O' ");
	TwAddVarRW(bar, "load_file_name", TW_TYPE_STDSTRING, &g_file_name_load, " group='I/O' ");
	TwAddButton(bar, "Load", LoadCB, NULL, " group='I/O' ");
}

// Hide/Show the TwBar and the HelpBar (the small icon in the bottom left corner)
void twBarVisible(bool b) {
	if (b) {
		TwDefine(" Settings visible=true ");
		TwDefine(" TW_HELP visible=true ");
	}
	else {
		TwDefine(" Settings visible=false ");
		TwDefine(" TW_HELP visible=false ");
	}
}

void refreshTwBar() {
	TwDeleteBar(bar);
	TwDraw();
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