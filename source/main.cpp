#include "main.h"

// Gets called when the windows is resized.
void resize_callback(int width, int height) {
    window_params.width = width;
	window_params.height = height;

    glViewport(0, 0, width, height);
	fbw.resize(width, height);

	camera.compute_projection_matrix();
}

void compute_height_map() {

	if (fractal_values.enable) {
		renderFractal(&tex_height,
			&noise_values,
			&fractal_values
		);
	} else {
		renderNoise(&tex_height, &noise_values);
	}

	box.setHeightTexture(tex_height);
	grid.setHeightTexture(tex_height);
	water.setHeightTexture(tex_height);
	water.setMirrorTexture(tex_mirror);
}

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
		sstm << "terrain-" << glfwGetTime() << ".terrain";
		g_file_name = sstm.str();
		g_file_name_load = g_file_name; // optional
	}

	writeFile(g_file_name, &noise_values, &fractal_values, &water_params);
}

void TW_CALL LoadCB(void * /*clientData*/)
{
	if (!g_file_name_load.compare("")) {
		// Empty name
		std::cout << "[Error] Cannot load from empty name" << endl;
	} else {
		loadFromFile(g_file_name_load, &noise_values, &fractal_values, &water_params);
	}

	// Update scene with the changes
	compute_height_map();
}

void initAntTwBar() {

	// Dummy variables that will be deleted and replaced by the ones used in our program


	TwInit(TW_OPENGL_CORE, NULL);
	// Needed to work with dynamic strings
	TwCopyStdStringToClientFunc(CopyStdStringToClient);
	TwWindowSize(window_params.width, window_params.height);
	bar = TwNewBar("Settings");

	/* Noise */

	TwEnumVal noise_type_array[] = { { NO_NOISE, "NO_NOISE" }, { RANDOM_NOISE, "RANDOM_NOISE" }, { PERLIN_NOISE, "PERLIN_NOISE" }, { WORLEY_NOISE, "WORLEY_NOISE" } };
	TwType noise_type_type = TwDefineEnum("NoiseType", noise_type_array, 4);
	TwAddVarCB(bar, "noise_type", noise_type_type, setIntParamCallback, getIntParamCallback, &noise_values.type, " group=Noise ");

	TwAddVarCB(bar, "noise_width", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &noise_values.width, " group=Noise step=1");
	TwAddVarCB(bar, "noise_height", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &noise_values.height, " group=Noise step=1");
	TwAddVarCB(bar, "noise_offset", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_values.offset, " group=Noise step=0.1");
	TwAddVarCB(bar, "noise_amplitude", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_values.amplitude, " group=Noise step=0.1");
	TwAddVarCB(bar, "seed", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_values.seed, " group=Noise step=0.001 min=0 max=1");

	TwEnumVal noise_effect_array[] = { { NO_EFFECT, "NO_EFFECT" }, { ABSOLUTE_VALUE, "ABSOLUTE_VALUE" }, { CLAMP_EXTREMAS, "CLAMP_EXTREMAS" }, { DISCRETIZE, "DISCRETIZE" } };
	TwType noise_effect_type = TwDefineEnum("NoiseEffect", noise_effect_array, 4);
	TwAddVarCB(bar, "noise_effect", noise_effect_type, setIntParamCallback, getIntParamCallback, &noise_values.effect, " group=Noise ");

	/* Fractal */

	TwEnumVal fractalEV[] = { { FBM, "FBM" }, { MULTIFRACTAL, "MULTIFRACTAL" } };

	TwType fractalType;
	fractalType = TwDefineEnum("FractalType", fractalEV, 2);
	TwAddVarCB(bar, "fractal_type", fractalType, setIntParamCallback, getIntParamCallback, &fractal_values.fractal_type, " group=Fractal ");

	TwAddVarCB(bar, "enable", TW_TYPE_BOOLCPP, setBoolParamCallback, getBoolParamCallback, &fractal_values.enable, " group=Fractal ");
	TwAddVarCB(bar, "H", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &fractal_values.H, " group=Fractal step=0.1");
	TwAddVarCB(bar, "lacunarity", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &fractal_values.lacunarity, " group=Fractal step=1 min=2");
	TwAddVarCB(bar, "octaves", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &fractal_values.octaves, " group=Fractal step=1 min=1");
	TwAddVarCB(bar, "fractal_offset", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &fractal_values.offset, " group=Fractal step=0.1");
	TwAddVarCB(bar, "fractal_amplitude", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &fractal_values.amplitude, " group=Fractal step=0.1");

	TwEnumVal fractal_effect_array[] = { { NO_EFFECT, "NO_EFFECT" }, { ABSOLUTE_VALUE, "ABSOLUTE_VALUE" }, { CLAMP_EXTREMAS, "CLAMP_EXTREMAS" }, { DISCRETIZE, "DISCRETIZE" } };
	TwType fractal_effect_type = TwDefineEnum("FractalEffect", fractal_effect_array, 4);
	TwAddVarCB(bar, "fractal_effect", fractal_effect_type, setIntParamCallback, getIntParamCallback, &fractal_values.fractal_effect, " group=Fractal ");

	/* Water */

	TwAddVarCB(bar, "enable", TW_TYPE_BOOLCPP, setBoolParamCallback, getBoolParamCallback, nullptr, " group=Water ");
	TwAddVarCB(bar, "height", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params.height, " group=Water step=0.1");
	TwAddVarCB(bar, "alpha", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params.transparency, " group=Water step=0.1 min=0, max=1");
	TwAddVarCB(bar, "depth_alpha_factor", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params.depth_alpha_factor, " group=Water step=0.1 min=0");
	TwAddVarCB(bar, "depth_color_factor", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params.depth_color_factor, " group=Water step=0.1 min=0");
	TwAddVarCB(bar, "reflection_factor", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params.reflection_factor, " group=Water step=0.1 min=0 max=1");
	TwAddVarCB(bar, "red", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params.color[0], " group=Water step=0.05 min=0 max=1");
	TwAddVarCB(bar, "green", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params.color[1], " group=Water step=0.05 min=0 max=1");
	TwAddVarCB(bar, "blue", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &water_params.color[2], " group=Water step=0.05 min=0 max=1");

	/* I/O */

	TwAddVarRW(bar, "save_file_name", TW_TYPE_STDSTRING, &g_file_name, " group='I/O' label='file_name (optional)' ");
	TwAddButton(bar, "Save", SaveCB, NULL, " group='I/O' ");
	TwAddVarRW(bar, "load_file_name", TW_TYPE_STDSTRING, &g_file_name_load, " group='I/O' ");
	TwAddButton(bar, "Load", LoadCB, NULL, " group='I/O' ");

	// Note: Callbacks are handled by the functions OnMousePos, OnMouseButton, etc...
}

void init(){
    // Sets background color.
    glClearColor(/*gray*/ .937,.937,.937, /*solid*/1.0);
    
    // Enable depth test.
    glEnable(GL_DEPTH_TEST);
    
	// All in main.h
	initParams();
	initTextures();
	initSceneObjects();

	compute_height_map();

#ifdef WITH_ANTTWEAKBAR

	initAntTwBar();
#endif

	check_error_gl();
}

// Gets called for every frame.
void display(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render the water reflect
	fbw.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		grid.draw(camera.get_view_matrix_mirrored(), camera.get_projection_matrix(), true);
	fbw.unbind();

	glViewport(0, 0, window_params.width, window_params.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH);
	glDisable(GL_ALPHA_TEST);

	fb_water_depth.bind();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		grid.draw(camera.get_view_matrix(), camera.get_projection_matrix(), false);
	fb_water_depth.unbind();
	

	grid.draw(camera.get_view_matrix(), camera.get_projection_matrix(), false);
	water.draw(camera.get_view_matrix(), camera.get_projection_matrix());
	box.draw(camera.get_view_matrix(), camera.get_projection_matrix());
	sky.draw(camera.get_view_matrix(), camera.get_projection_matrix());

	camera.move();

#ifdef WITH_ANTTWEAKBAR
	TwDraw();
#endif
}

void cleanup(){
#ifdef WITH_ANTTWEAKBAR
    TwTerminate();
#endif
}

int main(int, char**){
    glfwInitWindowSize(window_params.width, window_params.height);
    glfwCreateWindow("Terrain");
    glfwDisplayFunc(display);

	glfwSetWindowSizeCallback(OnWindowSize);
	glfwSetMouseButtonCallback(OnMouseButton);
	glfwSetMousePosCallback(OnMousePos);
	glfwSetMouseWheelCallback(OnMouseWheel);
	glfwSetKeyCallback(OnKey);
	glfwSetCharCallback(OnChar);

    init();
    glfwMainLoop();
    cleanup();
    return EXIT_SUCCESS;
}

// --- Callbacks --- //

// Callback function called by GLFW when a mouse button is clicked
void GLFWCALL OnMouseButton(int glfwButton, int glfwAction)
{
	if (!TwEventMouseButtonGLFW(glfwButton, glfwAction))   // Send event to AntTweakBar
	{
		// Event not handled by AntTweakBar, we handle it ourselves
		camera.mouse_button(glfwButton, glfwAction);
	}
}


// Callback function called by GLFW when mouse has moved
void GLFWCALL OnMousePos(int mouseX, int mouseY)
{
	if (!TwEventMousePosGLFW(mouseX, mouseY))  // Send event to AntTweakBar
	{
		camera.mouse_pos(mouseX, mouseY);
	}
}


// Callback function called by GLFW on mouse wheel event
void GLFWCALL OnMouseWheel(int pos)
{
	if (!TwEventMouseWheelGLFW(pos))   // Send event to AntTweakBar
	{
		// Nothing for the moment
	}
}


// Callback function called by GLFW on key event
void GLFWCALL OnKey(int glfwKey, int glfwAction)
{

	camera.keyboard(glfwKey, glfwAction);

	if (!TwEventKeyGLFW(glfwKey, glfwAction))  // Send event to AntTweakBar
	{
		if (glfwKey == GLFW_KEY_ESC && glfwAction == GLFW_PRESS) // Want to quit?
			glfwCloseWindow();
		else
		{
			// Nothing for the moment
		}
	}
}


// Callback function called by GLFW on char event
void GLFWCALL OnChar(int glfwChar, int glfwAction)
{
	if (!TwEventCharGLFW(glfwChar, glfwAction))    // Send event to AntTweakBar
	{
		// Nothing for the moment
	}
}


// Callback function called by GLFW when window size changes
void GLFWCALL OnWindowSize(int width, int height)
{
	// Send the new window size to AntTweakBar
	TwWindowSize(width, height);

	resize_callback(width, height);
}
