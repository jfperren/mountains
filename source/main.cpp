#include "icg_common.h"
#include "trackball/trackball.h"
#include "grid/grid.h"
#include "framebuffer/FrameBuffer.h"
#include "fullscreenquad/FullScreenQuad.h"

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

const float HEIGHT_MAP_HEIGHT = 300;
const float HEIGHT_MAP_WIDTH = 300;

FrameBuffer fb(WIDTH, HEIGHT);
FullScreenQuad fullScreenQuad;

// Constants
const float kZoomFactor = 2;

// Used to store old values in computation
mat4 old_trackball_matrix;
mat4 old_view_matrix;
float zoom_start_y;

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

void init(){
	// Sets background color.
	glClearColor(/*gray*/ .937, .937, .937, /*solid*/1.0);

	// Enable depth test.
	glEnable(GL_DEPTH_TEST);

	// view_matrix = LookAt(vec3(2.0f, 2.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	view_matrix = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, -4.0f)).matrix();

	trackball_matrix = mat4::Identity();

	height_map = fb.init();

	//GLuint color;
	// Initialize height_map properties
	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);
	glfwLoadTexture2D("grid/texture1D.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	grid.init();

	// Create fullScreenQuad on which we'll draw the noise
	fullScreenQuad.init();

	// Dummy variables that will be deleted and replaced by the ones used
	// in our program
	int a = 0;
	bool vs = true;
	bool gs = true;
	bool fs = true;
	float scale = 1.5f;

#ifdef WITH_ANTTWEAKBAR

	// Noise
	typedef enum { PERLIN, PERLIN2, PERLIN3, PERLIN4 } Noises;
	Noises noise = PERLIN;

	TwEnumVal noisesEV[] = { { PERLIN, "PERLIN" }, { PERLIN2, "PERLIN2" }, { PERLIN3, "PERLIN3" }, { PERLIN4, "PERLIN4" } };
	TwType noiseType;

	// Texture
	typedef enum { TEXTURE, TEXTURE2, TEXTURE3, TEXTURE4 } Textures;
	Textures texture = TEXTURE;

	TwEnumVal texturesEV[] = { { TEXTURE, "TEXTURE" }, { TEXTURE2, "TEXTURE2" }, { TEXTURE3, "TEXTURE3" }, { TEXTURE4, "TEXTURE4" } };
	TwType textureType;


	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT);
	bar = TwNewBar("Settings");


	TwAddVarRW(bar, "vs", TW_TYPE_BOOLCPP, &vs, " group='Shaders' label='vertex' key=v help='Toggle vertex shader.' ");
	TwAddVarRW(bar, "gs", TW_TYPE_BOOLCPP, &gs, " group='Shaders' label='geometry' key=g help='Toggle geometry shader.' ");
	TwAddVarRW(bar, "fs", TW_TYPE_BOOLCPP, &fs, " group='Shaders' label='fragment' key=f help='Toggle fragment shader.' ");

	TwAddVarRW(bar, "scale", TW_TYPE_FLOAT, &scale, " min=0 max=100 step=0.5 label='Height scale' ");

	noiseType = TwDefineEnum("NoiseType", noisesEV, 4);
	TwAddVarRW(bar, "Noise", noiseType, &noise, NULL);

	textureType = TwDefineEnum("TextureType", texturesEV, 4);
	TwAddVarRW(bar, "Texture", textureType, &texture, NULL);

	// Callbacks are handled by the functions OnMousePos, OnMouseButton, etc...
#endif

	//check_error_gl();
}

// Gets called for every frame.
void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw a quad on the ground.
	mat4 quad_model_matrix = mat4::Identity();

	///--- Render to FB
	fb.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fullScreenQuad.draw();
	fb.unbind();
	glViewport(0, 0, WIDTH, HEIGHT);

	grid.setColor(&color);
	grid.setHeightMap(&height_map);
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