#include "icg_common.h"
#include "trackball/trackball.h"
#include "grid/grid.h"
#include "framebuffer/Framebuffer.h"
#include "noise/NoiseQuad.h"
#include "noise/NoiseGenerator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <algorithm>

#ifdef WITH_ANTTWEAKBAR
#include <AntTweakBar.h>
TwBar *bar;
#endif

using namespace std;

Grid grid;

int WIDTH = 800;
int HEIGHT = 600;

mat4 projection_matrix;
mat4 view_matrix;
mat4 trackball_matrix;

Trackball trackball;

// Texture for noise
GLuint height_map;
// Texture for color
GLuint color;

// Constants
const float kZoomFactor = 2;

// Used to store old values in computation
mat4 old_trackball_matrix;
mat4 old_view_matrix;
float zoom_start_y;

// --- Variables for AntTweakBar ---

// --- Noise ---
NoiseQuad::NoiseValues noise_values;

// --- Fractal ---
NoiseQuad::FractalType fractal_type;
float fractal_amplitude;
float fractal_offset;
float fractal_H;
float fractal_lacunarity;
int fractal_octaves;
bool fractal_enable;

// --- I/O ---
string g_file_name = "";
string g_file_name_load = "";

/* ------------------------- */

// Dumps noise and fractal info to file_name
void writeFile(string file_name) {
	ofstream myfile(file_name);

	if (myfile.is_open()) {

		/* The order is important */

		/* Noise */
		myfile << "noise_type " << noise_values.noise_type << "\n";
		myfile << "noise_width " << noise_values.width << "\n";
		myfile << "noise_height " << noise_values.height << "\n";
		myfile << "noise_offset " << noise_values.offset << "\n";
		myfile << "noise_amplitude " << noise_values.amplitude << "\n";
		myfile << "noise_seed " << noise_values.seed << "\n";

		/* Fractal */
		if (fractal_enable) {
			myfile << "fractal_enable " << "true" << "\n";
			myfile << "fractal_H " << fractal_H << "\n";
			myfile << "fractal_lacunarity " << fractal_lacunarity << "\n";
			myfile << "fractal_octaves " << fractal_octaves << "\n";
			myfile << "fractal_offset " << fractal_offset << "\n";
			myfile << "fractal_amplitude " << fractal_amplitude << "\n";

		} else {
			myfile << "fractal_enable " << "false" << "\n";
		}

		myfile.close();
		cout << "Info: Data saved to " << file_name << endl;
	} else {
		cout << "Error: Could not save data: the file " << file_name << " could not be opened." << endl;
	}
}

void loadFromFile(string file_name) {
	string line;
	ifstream myfile(file_name);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			string str = line;

			// construct a stream from the string
			stringstream strstr(str);

			// use stream iterators to copy the stream to the vector as whitespace separated strings
			istream_iterator<string> it(strstr);
			istream_iterator<string> end;
			vector<string> results(it, end);

			// send the vector to stdout.
			ostream_iterator<string> oit(cout);
			copy(results.begin(), results.end(), oit);

			if (!results[0].compare("fractal_enable")) {
				if (!results[1].compare("true")) {
					fractal_enable = true;
				}
				else {
					fractal_enable = false;
				}
			}
			else if (!results[0].compare("fractal_H")) {
				fractal_H = ::atof(results[1].c_str());
			}
		}
		
		myfile.close();
		cout << "Info: Data loaded from " << file_name << endl;
	}
	else {
		cout << "Error: Could not load data: the file" << file_name << " could not be opened." << endl;
	}
}

mat4 OrthographicProjection(float left, float right, float bottom, float top, float near, float far){
    assert(right > left);
    assert(far > near);
    assert(top > bottom);
    mat4 ortho = mat4::Zero();
    ortho(0, 0) = 2.0f / (right - left);
    ortho(1, 1) = 2.0f / (top - bottom);
    ortho(2, 2) = -2.0f / (far - near);
    ortho(3, 3) = 1.0f;
    ortho(1, 3) = -(right + left) / (right - left);
    ortho(2, 3) = -(top + bottom) / (top - bottom);
    ortho(2, 3) = -(far + near) / (far - near);
    return ortho;
}

mat4 PerspectiveProjection(float fovy, float aspect, float near, float far){
    mat4 projection = mat4::Identity();

	float top = near * tan(1.0f / 2.0f * fovy);
	float right = aspect * top;

	projection <<
		near / right, 0.0f, 0.0f, 0.0f,
		0.0f, near / top, 0.0f, 0.0f,
		0.0f, 0.0f, (far + near) / (near - far), 2 * far*near / (near - far),
		0.0f, 0.0f, -1.0f, 0.0f;

    return projection;
}

mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
    vec3 z_cam = (eye - center).normalized();
    vec3 x_cam = up.cross(z_cam).normalized();
    vec3 y_cam = z_cam.cross(x_cam);

    mat3 R;
    R.col(0) = x_cam;
    R.col(1) = y_cam;
    R.col(2) = z_cam;

    mat4 look_at = mat4::Zero();
    look_at.block(0, 0, 3, 3) = R.transpose();
    look_at(3, 3) = 1.0f;
    look_at.block(0, 3, 3, 1) = -R.transpose() * (eye);
    return look_at;
}

// Gets called when the windows is resized.
void resize_callback(int width, int height) {
    WIDTH = width;
    HEIGHT = height;

    std::cout << "Window has been resized to " << WIDTH << "x" << HEIGHT << "." << std::endl;
    glViewport(0, 0, WIDTH, HEIGHT);

    GLfloat top = 1.0f;
    GLfloat right = (GLfloat)WIDTH / HEIGHT * top;
    //projection_matrix = OrthographicProjection(-right, right, -top, top, -10.0, 10.0f);
    projection_matrix = PerspectiveProjection(45.0f, (GLfloat)WIDTH / HEIGHT, 0.1f, 100.0f);
}

void compute_height_map() {

	if (fractal_enable) {
		if (fractal_type == NoiseQuad::FBM) {
			NoiseGenerator::generateFBM(&height_map,
				noise_values,
				fractal_amplitude,
				fractal_offset,
				fractal_H,
				fractal_lacunarity,
				fractal_octaves
				);
		} else {
			NoiseGenerator::generateMultiFractal(&height_map,
				noise_values,
				fractal_amplitude,
				fractal_offset,
				fractal_H,
				fractal_lacunarity,
				fractal_octaves
				);
		}
	} else {
		NoiseGenerator::renderNoise(&height_map, noise_values);
	}

	grid.setHeightMap(&height_map);
	glViewport(0, 0, WIDTH, HEIGHT);
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
		sstm << "mountain-" << glfwGetTime() << ".terrain";
		g_file_name = sstm.str();
		g_file_name_load = g_file_name; // optional
	}

	writeFile(g_file_name);
}

void TW_CALL LoadCB(void * /*clientData*/)
{
	if (!g_file_name_load.compare("")) {
		// Empty name
		cout << "Error: Cannot load from empty name" << endl;
	} else {
		loadFromFile(g_file_name_load);
	}
}




void initAntTwBar() {

	// Dummy variables that will be deleted and replaced by the ones used in our program


	TwInit(TW_OPENGL_CORE, NULL);
	// Needed to work with dynamic strings
	TwCopyStdStringToClientFunc(CopyStdStringToClient);
	TwWindowSize(WIDTH, HEIGHT);
	bar = TwNewBar("Settings");

	/* Shaders */

	//TwAddVarRW(bar, "vs", TW_TYPE_BOOLCPP, &vs, " group='Shaders' label='vertex' key=v help='Toggle vertex shader.' ");
	//TwAddVarRW(bar, "gs", TW_TYPE_BOOLCPP, &gs, " group='Shaders' label='geometry' key=g help='Toggle geometry shader.' ");
	//TwAddVarRW(bar, "fs", TW_TYPE_BOOLCPP, &fs, " group='Shaders' label='fragment' key=f help='Toggle fragment shader.' ");

	/* Noise */

	TwEnumVal noisesEV[] = { { NoiseQuad::NO_NOISE, "NO_NOISE" }, { NoiseQuad::RANDOM_NOISE, "RANDOM_NOISE" }, { NoiseQuad::PERLIN_NOISE, "PERLIN_NOISE" }, { NoiseQuad::PERLIN_NOISE_ABS, "PERLIN_NOISE_ABS" } };
	TwType noiseType;

	noiseType = TwDefineEnum("NoiseType", noisesEV, 4);
	TwAddVarCB(bar, "noise_type", noiseType, setIntParamCallback, getIntParamCallback, &noise_values.noise_type, " group=Noise ");

	TwAddVarCB(bar, "noise_width", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &noise_values.width, " group=Noise step=1");
	TwAddVarCB(bar, "noise_height", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &noise_values.height, " group=Noise step=1");
	TwAddVarCB(bar, "noise_offset", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_values.offset, " group=Noise step=0.1");
	TwAddVarCB(bar, "noise_amplitude", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_values.amplitude, " group=Noise step=0.1");
	TwAddVarCB(bar, "seed", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_values.seed, " group=Noise step=0.001 min=0 max=1");

	/* Fractal */

	TwAddVarCB(bar, "enable", TW_TYPE_BOOLCPP, setBoolParamCallback, getBoolParamCallback, &fractal_enable, " group=Fractal ");
	TwAddVarCB(bar, "H", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &fractal_H, " group=Fractal step=0.1");
	TwAddVarCB(bar, "lacunarity", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &fractal_lacunarity, " group=Fractal step=2");
	TwAddVarCB(bar, "octaves", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &fractal_octaves, " group=Fractal step=1");
	TwAddVarCB(bar, "fractal_offset", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &fractal_offset,  " group=Fractal step=0.1");
	TwAddVarCB(bar, "fractal_amplitude", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &fractal_amplitude, " group=Fractal step=0.1");

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
    
    view_matrix = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, -4.0f)).matrix();

    trackball_matrix = mat4::Identity();

	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);
	glfwLoadTexture2D("textures/texture1D.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	grid.init();

	glViewport(0, 0, WIDTH, HEIGHT);
	grid.setHeightMap(&height_map);

	// Init values

	// --- Noise ---
	noise_values.noise_type = NoiseQuad::PERLIN_NOISE;
	noise_values.height = 1;
	noise_values.width = 1;
	noise_values.offset = 0.0f;
	noise_values.amplitude = 1.0f;
	noise_values.seed = glfwGetTime();
	noise_values.seed -= floor(noise_values.seed);

	// --- Fractal ---
	fractal_amplitude = 1.0f;
	fractal_offset = 0.0f;
	fractal_H = 0.8f;
	fractal_lacunarity = 2;
	fractal_octaves = 8;
	fractal_enable = true;

	compute_height_map();


#ifdef WITH_ANTTWEAKBAR

	initAntTwBar();
#endif

	check_error_gl();
}

// Gets called for every frame.
void display(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw a quad on the ground.
	mat4 quad_model_matrix = mat4::Identity();

	grid.setColor(&color);
	grid.draw(trackball_matrix * quad_model_matrix, view_matrix, projection_matrix);

#ifdef WITH_ANTTWEAKBAR
	TwDraw();
#endif
}

void cleanup(){
#ifdef WITH_ANTTWEAKBAR
    TwTerminate();
#endif
}

// Transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 transform_screen_coords(int x, int y) {
    return vec2(2.0f * (float)x / WIDTH - 1.0f,
                1.0f - 2.0f * (float)y / HEIGHT);
}

void mouse_button(int button, int action) {
	// Rotation
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        int x_i, y_i;
        glfwGetMousePos(&x_i, &y_i);
        vec2 p = transform_screen_coords(x_i, y_i);
        trackball.begin_drag(p.x(), p.y());
		// Store the current state of the model matrix.
        old_trackball_matrix = trackball_matrix;  
    }

	// Zoom
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		// Get 2-d vector of the mouse position on screen in range [-1, 1]
		int x_i, y_i;
		glfwGetMousePos(&x_i, &y_i);
		vec2 p = transform_screen_coords(x_i, y_i);

		// Store y value & current state of the view matrix
		zoom_start_y = p[1];
		old_view_matrix = view_matrix;
	}
}

void mouse_pos(int x, int y) {
    // Rotation
    if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        vec2 p = transform_screen_coords(x, y);
		// Compute the new trackball_matrix
		trackball_matrix = trackball.drag(p[0], p[1]) * old_trackball_matrix;
    }

    // Zoom
    if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		// Get position on screen
		vec2 p = transform_screen_coords(x, y);
		// Zoom proportional to the distance
		float zoom = p[1] - zoom_start_y;
		// Apply it to translate the view matrix along the z-axis
		view_matrix = old_view_matrix * Eigen::Affine3f(Eigen::Translation3f(vec3(0.0f, 0.0f, zoom))).matrix();
    }
}

// Callback function called by GLFW when a mouse button is clicked
void GLFWCALL OnMouseButton(int glfwButton, int glfwAction)
{
	if (!TwEventMouseButtonGLFW(glfwButton, glfwAction))   // Send event to AntTweakBar
	{
		// Event not handled by AntTweakBar, we handle it ourselves
		mouse_button(glfwButton, glfwAction);
	}
}


// Callback function called by GLFW when mouse has moved
void GLFWCALL OnMousePos(int mouseX, int mouseY)
{
	if (!TwEventMousePosGLFW(mouseX, mouseY))  // Send event to AntTweakBar
	{
		mouse_pos(mouseX, mouseY);
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

int main(int, char**){
    glfwInitWindowSize(WIDTH, HEIGHT);
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