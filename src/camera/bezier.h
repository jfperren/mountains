#pragma once

#include "icg_common.h"
#include "../app/constants.h"
#include "bezier_curve.h";
#include "../camera/point.h"

class Bezier {

private:
	mat4 model = mat4::Identity();

	GLuint _pid_bezier;
	GLuint _pid_point;
	GLuint _pid_point_selection;

	BezierCurve cam_pos_curve;
	BezierCurve cam_look_curve;

	std::vector<ControlPoint> cam_pos_points;
	std::vector<ControlPoint> cam_look_points;
	int selected_point;

	int travel_time;
	int start_time;

public:
	void init(int travel_time=20);
	void cleanup();
	void display();
	void set_start_time(int start_time);
	int get_start_time();
	void set_travel_time(int travel_time);
	int get_travel_time();
	void pos_curve_sample_point(double t, vec3 &sample);
	void cam_look_sample_point(double t, vec3 &sample);
	void pos_curve_draw(const mat4& view, const mat4& projection);
	void cam_look_draw(const mat4& view, const mat4& projection);
};