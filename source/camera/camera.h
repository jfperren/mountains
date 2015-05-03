//#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "icg_common.h"
#include "../constants.h"
#include <math.h>

using namespace Eigen;

const vec3 START_CAM_UP  = vec3(0.0f, 1.0f, 0.0f);
const vec3 START_CAM_DIR = vec3(0.0f, -1.0f, -2.0f).normalized();
const vec3 START_CAM_POS = vec3(0.0f, 2.0f, 4.0f);

const float MAX_Y_CAM_DIR = 0.9;

const float DX = 0.01f;
const float DPHI = 1.0f;

const float NEAR = 0.1f;
const float FAR = 40.0f;

class Camera {

private:
	vec3 _cam_up;
	vec3 _cam_pos;
	vec3 _cam_dir;

	vec4 _old_cam_pos;
	vec4 _old_cam_dir;

	mat4 _projection_matrix;
	mat4 _view_matrix;

	mat4 _dir_matrix;

	vec3 _anchor_pos;

	std::map<int, bool> pressed_keys;

	WindowParams* _window_params;

public:
	// Constructor
	Camera(WindowParams* window_params);

	// Getters
	mat4 get_view_matrix();
	mat4 get_projection_matrix();
	mat4 get_view_matrix_mirrored();

	// (Re)Setters
	void compute_view_matrix();
	void compute_projection_matrix();

	// User Interaction
	void mouse_button(int button, int action);
	void mouse_pos(float x, float y);
	void keyboard(int key, int action);
	void move();

private:
	// Helpers
	vec2 transform_screen_coords(int x, int y);
};

#endif
