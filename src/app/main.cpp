#include "main.h"

// --- Params --- // 

WindowParams window_params{ WIDTH, HEIGHT };
NoiseParams noise_params;
DirtParams dirt_params;
ErosionParams erosion_params;
ShadingParams shading_params;
WaterParams water_params;
TextureParams texture_params;
GridParams grid_params;

AppParams app_params{
	&window_params,
	&grid_params,
	&noise_params,
	&dirt_params,
	&erosion_params,
	&texture_params,
	&shading_params,
	&water_params
};

// --- Scene Objects --- // 

Grid grid;
Water water;
Box box;
Sky sky;

// --- Buffers --- // 

Framebuffer fbw(WIDTH, HEIGHT);
Depthbuffer fb_water_depth(WIDTH, HEIGHT);

// --- Textures --- //

GLuint _tex_height;
GLuint _tex_dirt;
GLuint tex_mirror;
GLuint tex_water_depth;
GLuint tex_normal_map;

// --- Other --- //

NoiseGenerator noise_generator(&_tex_height, &_tex_dirt);
Camera camera(&window_params);

// Gets called when the windows is resized.
void resize_callback(int width, int height) {
    window_params.width = width;
	window_params.height = height;

    glViewport(0, 0, width, height);
	//fbw.resize(width, height);
	fb_water_depth.resize(width, height);

	camera.compute_projection_matrix();
}

void compute_height_map() {

	noise_generator.renderFractal();
	//noise_generator.erode();
	noise_generator.addDirt();

	box.set_height_texture(_tex_height);
	water.set_height_texture(_tex_height);
	water.set_mirror_texture(tex_mirror);
	water.set_normal_map(tex_normal_map);
}

void init(){
    // Sets background color.
    glClearColor(/*gray*/ .937,.937,.937, /*solid*/1.0);
    
    // Enable depth test.
    glEnable(GL_DEPTH_TEST);

	initParams();
	initTextures();
	initSceneObjects();

	noise_generator.init(&app_params);
	
	compute_height_map();

#ifdef WITH_ANTTWEAKBAR

	initAntTwBar(&app_params);
#endif
	check_error_gl();
}

// Gets called for every frame.
void display(){

	opengp::update_title_fps("Mountains");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render the water reflect
	fbw.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		grid.draw(camera.get_view_matrix_mirrored(), camera.get_projection_matrix(), true);
	fbw.unbind();

	glViewport(0, 0, window_params.width, window_params.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

// --- Helpers --- //

void initParams() {

	// --- Window ---
	window_params.width					= WIDTH;
	window_params.height				= HEIGHT;

	// --- Grid ---
	grid_params.enable					= true;
	grid_params.resolution				= 100;
	grid_params.length					= 2;
	grid_params.width					= 2;

	// --- Noise ---
	noise_params.noise_type				= PERLIN_NOISE;
	noise_params.fractal_type			= FBM;
	noise_params.noise_effect			= NO_EFFECT;
	noise_params.fractal_effect			= NO_EFFECT;
	noise_params.resolution				= 2048;
	noise_params.height					= 1;
	noise_params.width					= 1;
	noise_params.offset					= 0.0f;
	noise_params.amplitude				= 1.0f;
	noise_params.H						= 1.2f;
	noise_params.lacunarity				= 2;
	noise_params.octaves				= 12;
	noise_params.seed					= glfwGetTime();
	noise_params.seed 				   -= floor(noise_params.seed);

	// --- Dirt ---
	dirt_params.enable					= true;
	dirt_params.amount					= 0.26;
	dirt_params.max_height				= 0.2;
	dirt_params.max_slope				= 1.8;
	dirt_params.time					= 25;
	dirt_params.threshold				= 0.00;

	// --- Erosion ---
	erosion_params.deposition_rate		= 0.04;
	erosion_params.erosion_rate			= 0.04;
	erosion_params.rain_rate			= 0.05;
	erosion_params.evaporation_rate		= 0.02;
	
	erosion_params.sediment_capacity	= 0.08;
	erosion_params.direction_inertia	= 0.4;

	erosion_params.iterations			= 1;

	// --- Water ---
	water_params.height					= 0;
	water_params.color					= vec3(0.4f, 0.55f, 0.6f);
	water_params.depth_alpha_factor		= 1.4f;
	water_params.depth_color_factor		= 0.05f;
	water_params.transparency			= 0.4f;

	water_params.reflection_factor		= 0.25f;
	/*water_params.waves_speed = 0.04f;
	water_params.waves_tile_factor = 2.0;
	water_params.waves_amplitude = 1.0f;*/


	// --- Shading ---
	shading_params.enable				= true;
	shading_params.Ia					= vec3(0.7f, 0.7f, 0.7f);
	shading_params.Id					= vec3(0.3f, 0.3f, 0.3f);
	shading_params.light_pos			= vec3(2.0f, 2.0f, 2.0f);

	// --- Texture ---
	texture_params.texture_type			= SHADES;
	texture_params.sand_min_height		= -1;
	texture_params.sand_max_height		= 0.15f;
	texture_params.grass_max_height		= 1.0f;
	texture_params.sand_max_slope		= 0.92f;
	texture_params.grass_max_slope		= 0.9f;

	texture_params.grass_h_transition	= 3;
	texture_params.grass_s_transition	= 50;
	texture_params.sand_h_transition	= 5;
	texture_params.sand_s_transition	= 50;
}

void initSceneObjects() {

	box.init(&app_params);
	grid.init(&app_params, &_tex_height, &_tex_dirt);
	water.init(&app_params);

	tex_mirror = fbw.init_texture();
	fbw.init(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
	tex_water_depth = fb_water_depth.init_texture();
	fb_water_depth.init();

	
	water.set_depth_texture(tex_water_depth);
	sky.init();

	check_error_gl();
}

void initTextures() {
	glGenTextures(1, &tex_normal_map);
	glBindTexture(GL_TEXTURE_2D, tex_normal_map);
	glfwLoadTexture2D("textures/water/tex_normal_map.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &_tex_height);
	glBindTexture(GL_TEXTURE_2D, _tex_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &_tex_dirt);
	glBindTexture(GL_TEXTURE_2D, _tex_dirt);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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