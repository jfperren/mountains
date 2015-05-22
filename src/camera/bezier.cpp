#include "bezier.h"

void Bezier::set_start_time(int start_time) {
	this->start_time = start_time;
}

int Bezier::get_start_time() {
	return this->start_time;
}

void Bezier::set_travel_time(int travel_time) {
	this->travel_time = travel_time;
}

int Bezier::get_travel_time() {
	return this->travel_time;
}

void Bezier::pos_curve_sample_point(double t, vec3 &sample) {
	cam_pos_curve.sample_point(t, sample);
}

void Bezier::cam_look_sample_point(double t, vec3 &sample) {
	cam_look_curve.sample_point(t, sample);
}

void Bezier::pos_curve_draw(const mat4& view, const mat4& projection) {
	cam_pos_curve.draw(model, view, projection);
}
void Bezier::cam_look_draw(const mat4& view, const mat4& projection) {
	cam_look_curve.draw(model, view, projection);
}

void Bezier::draw_cam_pos_points(const mat4& view, const mat4& projection) {
	for (unsigned int i = 0; i < cam_pos_points.size(); i++) {
		cam_pos_points[i].draw(model, view, projection);
	}
}

void Bezier::draw_cam_look_points(const mat4& view, const mat4& projection) {
	for (unsigned int i = 0; i < cam_look_points.size(); i++) {
		cam_look_points[i].draw(model, view, projection);
	}
}

void Bezier::init(int travel_time){
	
	this->travel_time = travel_time;

	/// Compile the shaders here to avoid the duplication
	_pid_bezier = opengp::load_shaders("camera/bezier_vshader.glsl", "camera/bezier_fshader.glsl");
	if (!_pid_bezier) exit(EXIT_FAILURE);

	_pid_point = opengp::load_shaders("camera/point_vshader.glsl", "camera/point_fshader.glsl");
	if (!_pid_point) exit(EXIT_FAILURE);

	_pid_point_selection = opengp::load_shaders("camera/point_selection_vshader.glsl", "camera/point_selection_fshader.glsl");
	if (!_pid_point_selection) exit(EXIT_FAILURE);

	//--- init cam_pos_curve
	cam_pos_curve.init(_pid_bezier);

	// Add points
	cam_pos_points.push_back(ControlPoint(0.3, 0.5, -1.3, 0));
	cam_pos_points.push_back(ControlPoint(0.19, 0.26, -0.92, 1));
	cam_pos_points.push_back(ControlPoint(0.11, 0.10, -0.45, 2));
	cam_pos_points.push_back(ControlPoint(0.0, 0.0, 0.0, 3));
	cam_pos_points.push_back(ControlPoint(-0.11, -0.10, 0.45, 4));
	cam_pos_points.push_back(ControlPoint(-0.16, 0.92, -0.11, 5));
	cam_pos_points.push_back(ControlPoint(1.5, 0.15, 0.57, 6));

	for (unsigned int i = 0; i < cam_pos_points.size(); i++) {
		cam_pos_points[i].init(_pid_point, _pid_point_selection);
	}

	cam_pos_curve.set_points(cam_pos_points);

	///--- init cam_look_curve
	cam_look_curve.init(_pid_bezier);

	// Add points
	cam_look_points.push_back(ControlPoint(0, 0, 0.25, 7));
	cam_look_points.push_back(ControlPoint(0.17, 0.51, 0.24, 8));
	cam_look_points.push_back(ControlPoint(0.0, 0.89, 0.27, 9));
	cam_look_points.push_back(ControlPoint(0.0, 0, 0.25, 10));

	for (unsigned int i = 0; i < cam_look_points.size(); i++) {
		cam_look_points[i].init(_pid_point, _pid_point_selection);
	}

	cam_look_curve.set_points(cam_look_points);

	selected_point = -1;
}

void Bezier::cleanup() {
	glDeleteProgram(_pid_bezier);
	glDeleteProgram(_pid_point);
	glDeleteProgram(_pid_point_selection);
}

void Bezier::display() {
	
}