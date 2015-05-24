#include "main.h"

// --- Params --- // 

WindowParams window_params{ WIDTH, HEIGHT };
NoiseParams noise_params;
SnowParams snow_params;
ErosionParams erosion_params;
ShadingParams shading_params;
WaterParams water_params;
TextureParams texture_params;
GridParams grid_params;

AppParams app_params{
	&window_params,
	&grid_params,
	&noise_params,
	&snow_params,
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

Generalbuffer fb_mirror;
Generalbuffer fb_water_depth;
Generalbuffer fb_shadow;

// --- Textures --- //

GLuint* _tex_height;
GLuint* _tex_snow;
GLuint* _tex_dirt;

GLuint* _tex_mirror;
GLuint* _tex_water_depth;
GLuint* _tex_shadow;

// --- Other --- //

Terrain terrain;
Camera camera(&window_params);

void initBuffers() {
	
	fb_mirror.init(1);
	fb_mirror.genTextures();
	fb_mirror.setFormat(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
	
	fb_mirror.setSize(WIDTH, HEIGHT);
	fb_mirror.genTextureImages();
	fb_mirror.genRenderbuffer();
	fb_mirror.genFramebuffer(BUFFER_ATTACHMENT_0, 1);
	
	fb_water_depth.init(1);
	fb_water_depth.genTextures();
	fb_water_depth.setFormat(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);

	fb_water_depth.setSize(WIDTH, HEIGHT);
	fb_water_depth.genTextureImages();
	fb_water_depth.genFramebuffer(BUFFER_ATTACHMENT_DEPTH, 1);

	fb_shadow.init(1);
	fb_shadow.genTextures(true);
	fb_shadow.setFormat(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);

	fb_shadow.setSize(WIDTH, HEIGHT);
	fb_shadow.genTextureImages();
	fb_shadow.genFramebuffer(BUFFER_ATTACHMENT_DEPTH, 1);

	check_error_gl();
}

// Gets called when the windows is resized.
void resize_callback(int width, int height) {
	
	window_params.width = width;
	window_params.height = height;
	/*
	fb_mirror.setSize(width, height);
	fb_mirror.genTextureImages();
	fb_mirror.wrap(BUFFER_ATTACHMENT_0, 1);

	fb_water_depth.setSize(width, height);
	fb_water_depth.genTextureImages();
	fb_water_depth.wrap(BUFFER_ATTACHMENT_0, 1);*/

	camera.compute_projection_matrix();

	glViewport(0, 0, width, height);
}

void compute_height_map() {
	
	terrain.resize();
	terrain.renderFractal();
	terrain.addSnow();

}

void init(){
    // Sets background color.
    glClearColor(/*gray*/ .937,.937,.937, /*solid*/1.0);
	
    // Enable depth test.
    glEnable(GL_DEPTH_TEST);

	initParams();

	terrain.init(&app_params);
	
	initBuffers();
	initTextures();
	initSceneObjects();
	
	compute_height_map();
	
	resize_callback(WIDTH, HEIGHT);

#ifdef WITH_ANTTWEAKBAR

	initAntTwBar(&app_params);
#endif
	check_error_gl();
}

// Gets called for every frame.
void display(){
	
	opengp::update_title_fps("Mountains");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
	// 1. Shadow

	fb_shadow.bind(BUFFER_ATTACHMENT_0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		grid.draw(camera.get_view_matrix(), shading_params.get_projection_matrix(), camera.get_view_matrix(), shading_params.get_projection_matrix(), ILLUMINATE);
	fb_shadow.unbind();

	// 2. Water Reflects

	fb_mirror.bind(BUFFER_ATTACHMENT_0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	grid.draw(camera.get_view_matrix_mirrored(), shading_params.get_projection_matrix(), camera.get_view_matrix(), shading_params.get_projection_matrix(), ONLY_REFLECT);
	fb_mirror.unbind();

	check_error_gl();

	// 3. Water depth

	fb_water_depth.bind(BUFFER_ATTACHMENT_0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//box.draw(camera.get_view_matrix(), camera.get_projection_matrix());
	grid.draw(camera.get_view_matrix(), shading_params.get_projection_matrix(), camera.get_view_matrix(), shading_params.get_projection_matrix(), NORMAL);
	fb_water_depth.unbind();

	// 4. Scene

	grid.draw(camera.get_view_matrix(), shading_params.get_projection_matrix(), camera.get_view_matrix(), shading_params.get_projection_matrix(), NORMAL);
	water.draw(camera.get_view_matrix(), shading_params.get_projection_matrix());
	box.draw(camera.get_view_matrix(), shading_params.get_projection_matrix());
	sky.draw(camera.get_view_matrix(), shading_params.get_projection_matrix());
	camera.move();

	*/
	
	// 1. Shadow

	fb_shadow.bind(BUFFER_ATTACHMENT_0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		grid.draw(camera.get_view_matrix(), camera.get_projection_matrix(), shading_params.get_view_matrix(), shading_params.get_projection_matrix(), ILLUMINATE);
	fb_shadow.unbind();

	// 2. Water Reflects

	fb_mirror.bind(BUFFER_ATTACHMENT_0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		grid.draw(camera.get_view_matrix_mirrored(), camera.get_projection_matrix(), shading_params.get_view_matrix(), shading_params.get_projection_matrix(), ONLY_REFLECT);
	fb_mirror.unbind();

	check_error_gl();
	
	// 3. Water depth
	
	fb_water_depth.bind(BUFFER_ATTACHMENT_0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//box.draw(camera.get_view_matrix(), camera.get_projection_matrix());
		grid.draw(camera.get_view_matrix(), camera.get_projection_matrix(), shading_params.get_view_matrix(), shading_params.get_projection_matrix(), NORMAL);
	fb_water_depth.unbind();
	
	// 4. Scene

	grid.draw(camera.get_view_matrix(), camera.get_projection_matrix(), shading_params.get_view_matrix(), shading_params.get_projection_matrix(), NORMAL);
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

	// --- snow ---
	snow_params.enable					= true;
	snow_params.amount					= 0.02;
	snow_params.max_amount				= 2;
	snow_params.min_height				= 0.3;
	snow_params.max_slope				= 2;
	snow_params.threshold				= 0.001;
	snow_params.slide_time				= 25;
	snow_params.melt_time				= 10;
	snow_params.smooth_time				= 10;

	// --- Erosion ---
	erosion_params.deposition_rate		= 0.04;
	erosion_params.erosion_rate			= 0.04;
	erosion_params.rain_rate			= 0.05;
	erosion_params.evaporation_rate		= 0.02;
	
	erosion_params.sediment_capacity	= 0.08;
	erosion_params.direction_inertia	= 0.4;

	erosion_params.iterations			= 1;

	// --- Water ---
	water_params.enable					= true;
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
	shading_params.enable_phong			= true;
	shading_params.enable_shadow		= true;
	shading_params.shadow_intensity		= 0.5;
	shading_params.Ia					= vec3(0.7f, 0.7f, 0.7f);
	shading_params.Id					= vec3(0.3f, 0.3f, 0.3f);
	shading_params.light_pos			= vec3(0.5f, 1.0f, 0.5f);
	shading_params.far					= 4;
	shading_params.near					= -4;

	// --- Texture ---
	texture_params.texture_type			= TEXTURE;
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
	box.setHeightTexture(_tex_height);
	box.setWaterDepthTexture(_tex_water_depth);

	grid.init(&app_params);
	grid.setTexHeight(_tex_height);
	grid.setTexSnow(_tex_snow);
	grid.setTexDirt(_tex_dirt);
	grid.setTexShadow(_tex_shadow);

	water.init(&app_params);
	water.setHeightTexture(_tex_height);
	water.setMirrorTexture(_tex_mirror);
	water.setDepthTexture(_tex_water_depth);
	
	sky.init();
	
	check_error_gl();
}

void initTextures() {
	
	_tex_height = terrain.getHeightTexture();
	_tex_dirt = terrain.getDirtTexture();
	_tex_snow = terrain.getSnowTexture();

	_tex_shadow = fb_shadow.getTexture(0);
	_tex_mirror = fb_mirror.getTexture(0);
	_tex_water_depth = fb_water_depth.getTexture(0);
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