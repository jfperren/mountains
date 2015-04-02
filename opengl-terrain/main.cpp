#include "icg_common.h"
#include "cube.h"
#include "trackball.h"
#include "_grid/grid.h"

using namespace std;

Cube cube;
Grid grid;

int WIDTH = 800;
int HEIGHT = 600;

mat4 projection_matrix;
mat4 view_matrix;
mat4 trackball_matrix;

Trackball trackball;

mat4 OrthographicProjection(float left, float right, float bottom, float top, float near, float far){
    assert(right > left);
    assert(far > near);
    assert(top > bottom);
    mat4 ortho = mat4::Zero();
    ortho(0, 0) = 2.0f / (right - left);
    ortho(1, 1) = 2.0f / (top - bottom);
    ortho(2, 2) = -2.0f / (far - near);
    ortho(3, 3) = 1.0f;
    ortho(0, 3) = -(right + left) / (right - left);
    ortho(1, 3) = -(top + bottom) / (top - bottom);
    ortho(2, 3) = -(far + near) / (far - near);
    return ortho;
}

mat4 PerspectiveProjection(float fovy, float aspect, float near, float far){
    // TODO 1: Create a perspective projection matrix given the field of view,
    // aspect ratio, and near and far plane distances.
    mat4 projection = mat4::Identity();
    return projection;
}

mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
    // We need a function that converts from world coordinates into camera coordiantes.
    //
    // Cam coords to world coords is given by:
    // X_world = R * X_cam + eye
    //
    // inverting it leads to:
    //
    // X_cam = R^T * X_world - R^T * eye
    //
    // Or as a homogeneous matrix:
    // [ r_00 r_10 r_20 -r_0*eye
    //   r_01 r_11 r_21 -r_1*eye
    //   r_02 r_12 r_22 -r_2*eye
    //      0    0    0        1 ]

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

    // TODO 1: Use a perspective projection instead;
    //projection_matrix = PerspectiveProjection(45.0f, (GLfloat)WIDTH / HEIGHT, 0.1f, 100.0f);
    GLfloat top = 1.0f;
    GLfloat right = (GLfloat)WIDTH / HEIGHT * top;
    projection_matrix = OrthographicProjection(-right, right, -top, top, -10.0, 10.0f);
}

void init(){
    // Sets background color.
    glClearColor(/*gray*/ .937,.937,.937, /*solid*/1.0);
    
    cube.init();
    grid.init();

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);

    // TODO 3: Once you use the trackball, you should use a view matrix that
    // looks straight down the -z axis. Otherwise the trackball's rotation gets
    // applied in a rotated coordinate frame.
    // Uncomment lower line to achieve this.
    view_matrix = LookAt(vec3(2.0f, 2.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    //view_matrix = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, -4.0f)).matrix();

    trackball_matrix = mat4::Identity();
    check_error_gl();
}

// Gets called for every frame.
void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Scaling matrix to scale the cube down to a reasonable size.
    mat4 cube_scale;
    cube_scale << 0.25f, 0.0f,  0.0f,  0.0f,
                  0.0f,  0.25f, 0.0f,  0.0f,
                  0.0f,  0.0f,  0.25f, 0.0f,
                  0.0f,  0.0f,  0.0f,  1.0f;
    const float time = glfwGetTime();
    mat4 cube_trafo = (Eigen::Affine3f(Eigen::AngleAxisf(2.0f * time, vec3::UnitY())) *
                       Eigen::Affine3f(Eigen::Translation3f(vec3(0.75f, 0.0f, 0.0f))) *
                       Eigen::Affine3f(Eigen::AngleAxisf(2.0f * time, vec3::UnitY()))).matrix();
    //mat4 cube_trafo = Eigen::Affine3f(Eigen::AngleAxisf(time, vec3::UnitY())).matrix();
    mat4 cube_model_matrix = cube_trafo * cube_scale;

    cube.draw(trackball_matrix * cube_model_matrix, view_matrix, projection_matrix);

    // Draw a quad on the ground.
    mat4 quad_model_matrix = Eigen::Affine3f(Eigen::Translation3f(vec3(0.0f, -0.25f, 0.0f))).matrix();
    grid.draw(trackball_matrix * quad_model_matrix, view_matrix, projection_matrix, time);

    check_error_gl();
}

// Transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 transform_screen_coords(int x, int y) {
    return vec2(2.0f * (float)x / WIDTH - 1.0f,
                1.0f - 2.0f * (float)y / HEIGHT);
}

mat4 old_trackball_matrix;

void mouse_button(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        int x_i, y_i;
        glfwGetMousePos(&x_i, &y_i);
        vec2 p = transform_screen_coords(x_i, y_i);
        trackball.begin_drag(p.x(), p.y());
        old_trackball_matrix = trackball_matrix;  // Store the current state of the model matrix.
    }
}

void mouse_pos(int x, int y) {
    if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        vec2 p = transform_screen_coords(x, y);
        // TODO 3: Cacuclate 'trackball_matrix' given the return value of
        // trackball.drag(...) and the value stored in 'old_trackball_matrix'.
        // See also the mouse_button(...) function.
        //trackball_matrix = ...
    }

    // Zoom
    if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        // TODO 4: Implement zooming. When the right mouse button is pressed,
        // moving the mouse cursor up and down (along the screen's y axis)
        // should zoom out and it. For that you have to update the current
        // 'view_matrix' with a translation along the z axis.
        //view_matrix = ...
    }
}

int main(int, char**){
    glfwInitWindowSize(WIDTH, HEIGHT);
    glfwCreateWindow("Trackball");
    glfwDisplayFunc(display);
    glfwSetWindowSizeCallback(&resize_callback);
    glfwSetMouseButtonCallback(mouse_button);
    glfwSetMousePosCallback(mouse_pos);
    init();
    glfwMainLoop();
    return EXIT_SUCCESS;
}
