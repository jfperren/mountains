#include "icg_common.h"
#include "trackball/Trackball.h"
#include "grid/Grid.h"
#include "framebuffer/Framebuffer.h"
#include "framebuffer/FramebufferWater.h"
#include "noise/NoiseQuad.h"
#include "noise/NoiseGenerator.h"
#include "water/Water.h"
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
Water water;

int WIDTH = 800;
int HEIGHT = 600;

mat4 projection_matrix;
mat4 view_matrix;
mat4 trackball_matrix;

Trackball trackball;

FramebufferWater fbw(WIDTH, HEIGHT);

// Texture for noise
GLuint height_map;
// Texture for color
GLuint color;

// Constants
const float kZoomFactor = 2;
const string kHeaderString = "HEADER: terrain data";

// Used to store old values in computation
mat4 old_trackball_matrix;
mat4 old_view_matrix;
float zoom_start_y;

// --- Variables for AntTweakBar ---

// --- Noise ---
NoiseQuad::NoiseValues noise_values;

// --- Fractal ---
NoiseQuad::FractalValues fractal_values;
bool fractal_enable; 

// --- I/O ---
string g_file_name = "";
string g_file_name_load = "";

/* ------------------------- */

// Dumps noise and fractal info to file_name
void writeFile(string file_name) {
	ofstream myfile(file_name);

	if (myfile.is_open()) {

		/* Write header */
		myfile << kHeaderString << endl;

		/* The order is not important */

		/* Noise */
		myfile << "noise_type " << noise_values.noise_type << endl;
		myfile << "noise_width " << noise_values.width << endl;
		myfile << "noise_height " << noise_values.height << endl;
		myfile << "noise_offset " << noise_values.offset << endl;
		myfile << "noise_amplitude " << noise_values.amplitude << endl;
		myfile << "noise_seed " << noise_values.seed << endl;

		/* Fractal */
		if (fractal_enable) {
			myfile << "fractal_enable " << "true" << endl;
			myfile << "fractal_H " << fractal_values.H << endl;
			myfile << "fractal_lacunarity " << fractal_values.lacunarity << endl;
			myfile << "fractal_octaves " << fractal_values.octaves << endl;
			myfile << "fractal_offset " << fractal_values.offset << endl;
			myfile << "fractal_amplitude " << fractal_values.amplitude << endl;

		} else {
			myfile << "fractal_enable " << "false" << endl;
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

		int line_no = 0;

		while (getline(myfile, line))
		{
			line_no++;

			if (line_no == 1) {
				if (line.compare(kHeaderString)) {
					// the first line doesn't match the header -> illegal format
					cout << "Error: Illegal header. Aborting load." << endl;
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
			ostream_iterator<string> oit(cout);
			copy(results.begin(), results.end(), oit);

			/* Load fractal */
			if (!results[0].compare("fractal_enable")) {
				if (!results[1].compare("true")) {
					fractal_enable = true;
				}
				else {
					fractal_enable = false;
				}
			} else if (!results[0].compare("fractal_H")) {
				fractal_values.H = ::atof(results[1].c_str());
			} else if (!results[0].compare("fractal_lacunarity")) {
				fractal_values.lacunarity = ::atoi(results[1].c_str());
			} else if (!results[0].compare("fractal_octaves")) {
				fractal_values.octaves = ::atoi(results[1].c_str());
			} else if (!results[0].compare("fractal_offset")) {
				fractal_values.offset = ::atof(results[1].c_str());
			} else if (!results[0].compare("fractal_amplitude")) {
				fractal_values.amplitude = ::atof(results[1].c_str());
			}	/* Load noise */
			else if (!results[0].compare("noise_type")) {
				int type = ::atoi(results[1].c_str());
				switch (type) {
				case 0: noise_values.noise_type = NoiseQuad::COPY_TEXTURE;
					break;
				case 1: noise_values.noise_type = NoiseQuad::NO_NOISE;
					break;
				case 2: noise_values.noise_type = NoiseQuad::RANDOM_NOISE;
					break;
				case 3: noise_values.noise_type = NoiseQuad::PERLIN_NOISE;
					break;
				case 4: noise_values.noise_type = NoiseQuad::PERLIN_NOISE_ABS;
					break;
				default:
					cout << "Error: Unkown NoiseType" << endl;
					break;
				}

			} else if (!results[0].compare("noise_width")) {
				noise_values.width = ::atoi(results[1].c_str());
			} else if (!results[0].compare("noise_height")) {
				noise_values.height = ::atoi(results[1].c_str());
			} else if (!results[0].compare("noise_offset")) {
				noise_values.offset = ::atof(results[1].c_str());
			} else if (!results[0].compare("noise_amplitude")) {
				noise_values.amplitude = ::atof(results[1].c_str());
			} else if (!results[0].compare("noise_seed")) {
				noise_values.seed = ::atof(results[1].c_str());
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
		NoiseGenerator::renderFractal(&height_map,
			noise_values,
			fractal_values
		);
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

	// Update scene with the changes
	compute_height_map();
}




void initAntTwBar() {

	// Dummy variables that will be deleted and replaced by the ones used in our program


	TwInit(TW_OPENGL_CORE, NULL);
	// Needed to work with dynamic strings
	TwCopyStdStringToClientFunc(CopyStdStringToClient);
	TwWindowSize(WIDTH, HEIGHT);
	bar = TwNewBar("Settings");

	/* Noise */

	TwEnumVal noise_type_array[] = { { NoiseQuad::NO_NOISE, "NO_NOISE" }, { NoiseQuad::RANDOM_NOISE, "RANDOM_NOISE" }, { NoiseQuad::PERLIN_NOISE, "PERLIN_NOISE" }, { NoiseQuad::WORLEY_NOISE, "WORLEY_NOISE" } };
	TwType noise_type_type = TwDefineEnum("NoiseType", noise_type_array, 4);
	TwAddVarCB(bar, "noise_type", noise_type_type, setIntParamCallback, getIntParamCallback, &noise_values.noise_type, " group=Noise ");

	TwAddVarCB(bar, "noise_width", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &noise_values.width, " group=Noise step=1");
	TwAddVarCB(bar, "noise_height", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &noise_values.height, " group=Noise step=1");
	TwAddVarCB(bar, "noise_offset", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_values.offset, " group=Noise step=0.1");
	TwAddVarCB(bar, "noise_amplitude", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_values.amplitude, " group=Noise step=0.1");
	TwAddVarCB(bar, "seed", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &noise_values.seed, " group=Noise step=0.001 min=0 max=1");

	TwEnumVal noise_effect_array[] = { { NoiseQuad::NO_EFFECT, "NO_EFFECT" }, { NoiseQuad::ABSOLUTE_VALUE, "ABSOLUTE_VALUE" }, { NoiseQuad::CLAMP_EXTREMAS, "CLAMP_EXTREMAS" }, { NoiseQuad::DISCRETIZE, "DISCRETIZE" } };
	TwType noise_effect_type = TwDefineEnum("NoiseEffect", noise_effect_array, 4);
	TwAddVarCB(bar, "noise_effect", noise_effect_type, setIntParamCallback, getIntParamCallback, &noise_values.noise_effect, " group=Noise ");

	/* Fractal */

	TwEnumVal fractalEV[] = { { NoiseQuad::FBM, "FBM" }, { NoiseQuad::MULTIFRACTAL, "MULTIFRACTAL" } };

	TwType fractalType;
	fractalType = TwDefineEnum("FractalType", fractalEV, 2);
	TwAddVarCB(bar, "fractal_type", fractalType, setIntParamCallback, getIntParamCallback, &fractal_values.fractal_type, " group=Fractal ");

	TwAddVarCB(bar, "enable", TW_TYPE_BOOLCPP, setBoolParamCallback, getBoolParamCallback, &fractal_enable, " group=Fractal ");
	TwAddVarCB(bar, "H", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &fractal_values.H, " group=Fractal step=0.1");
	TwAddVarCB(bar, "lacunarity", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &fractal_values.lacunarity, " group=Fractal step=1 min=2");
	TwAddVarCB(bar, "octaves", TW_TYPE_INT32, setIntParamCallback, getIntParamCallback, &fractal_values.octaves, " group=Fractal step=1 min=1");
	TwAddVarCB(bar, "fractal_offset", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &fractal_values.offset, " group=Fractal step=0.1");
	TwAddVarCB(bar, "fractal_amplitude", TW_TYPE_FLOAT, setFloatParamCallback, getFloatParamCallback, &fractal_values.amplitude, " group=Fractal step=0.1");

	TwEnumVal fractal_effect_array[] = { { NoiseQuad::NO_EFFECT, "NO_EFFECT" }, { NoiseQuad::ABSOLUTE_VALUE, "ABSOLUTE_VALUE" }, { NoiseQuad::CLAMP_EXTREMAS, "CLAMP_EXTREMAS" }, { NoiseQuad::DISCRETIZE, "DISCRETIZE" } };
	TwType fractal_effect_type = TwDefineEnum("FractalEffect", fractal_effect_array, 4);
	TwAddVarCB(bar, "fractal_effect", fractal_effect_type, setIntParamCallback, getIntParamCallback, &fractal_values.fractal_effect, " group=Fractal ");

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	grid.init();
	

	GLuint water_reflect_texture = fbw.init();
	water.init(water_reflect_texture);

	glViewport(0, 0, WIDTH, HEIGHT);
	grid.setHeightMap(&height_map);

	// Init values

	// --- Noise ---
	noise_values.noise_type = NoiseQuad::PERLIN_NOISE;
	noise_values.noise_effect = NoiseQuad::NO_EFFECT;
	noise_values.height = 1;
	noise_values.width = 1;
	noise_values.offset = 0.0f;
	noise_values.amplitude = 1.0f;
	noise_values.seed = glfwGetTime();
	noise_values.seed -= floor(noise_values.seed);

	// --- Fractal ---
	fractal_values.fractal_type = NoiseQuad::FBM;
	fractal_values.fractal_effect = NoiseQuad::NO_EFFECT;
	fractal_values.amplitude = 1.0f;
	fractal_values.offset = 0.0f;
	fractal_values.H = 0.8f;
	fractal_values.lacunarity = 2;
	fractal_values.octaves = 6;
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

	float ratio = WIDTH / (float)HEIGHT;
	static mat4 projection = Eigen::perspective(45.0f, ratio, 0.1f, 10.0f);
	vec3 cam_pos(2.0f, 2.0f, 2.0f);
	vec3 cam_look(0.0f, 0.0f, 0.0f);
	vec3 cam_up(0.0f, 0.0f, 1.0f);
	mat4 view = Eigen::lookAt(cam_pos, cam_look, cam_up);
	mat4 VP = projection * view;

	// Mirror camera position
	vec3 cam_pos_mirrored(cam_pos[0], cam_pos[1], -cam_pos[2]);
	vec3 cam_look_mirrored = cam_look;
	vec3 cam_up_mirrored = cam_up;

	// Create new VP for mirrored camera
	mat4 view_mirrored = Eigen::lookAt(cam_pos_mirrored, cam_look_mirrored, cam_up_mirrored);
	mat4 VP_mirrored = projection * view_mirrored;

	// Render the cube using the mirrored camera in the frame buffer
	grid.setColor(&color);
	grid.draw(trackball_matrix, view_matrix, projection_matrix);

	//fbw.bind();
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//grid.draw(trackball_matrix, view_mirrored, projection_matrix);
	//fbw.unbind();

	grid.draw(trackball_matrix, view_matrix, projection_matrix);
	water.draw(trackball_matrix, view_matrix, projection_matrix);

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
