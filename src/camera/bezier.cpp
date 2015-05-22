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

void Bezier::init(int width, int height, int travel_time){

	this->width = width;
	this->height = height;
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

void Bezier::render_selection(const mat4 &view, const mat4 &projection) {
	glViewport(0, 0, width, height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (unsigned int i = 0; i < cam_pos_points.size(); i++) {
		cam_pos_points[i].draw_selection(model, view, projection);
	}

	for (unsigned int i = 0; i < cam_look_points.size(); i++) {
		cam_look_points[i].draw_selection(model, view, projection);
	}
}

vec2 Bezier::transform_xy_screen(int x, int y) {
	return vec2(2.0f * (float)x / width - 1.0f,
		1.0f - 2.0f * (float)y / height);
}

bool Bezier::unproject(int mouse_x, int mouse_y, vec3 &p, const mat4 &view, const mat4 &projection) {
	// Compute MVP and MVP_inv
	mat4 MVP = projection * view * model;
	mat4 MVP_inv = MVP.inverse();
	// Get point in space
	vec4 p_camera = MVP * vec4(p.x(), p.y(), p.z(), 1.0);
	p_camera = p_camera / p_camera.w();
	// Get current depth of the point
	float depth = p_camera.z();
	// Get mouse coordinates in screen coordinates
	vec2 xy_screen = transform_xy_screen(mouse_x, mouse_y);
	// Get point in scene and re-normalize w to 1
	vec4 p_xyzw = MVP_inv * vec4(xy_screen.x(), xy_screen.y(), depth, 1.0);
	p_xyzw = p_xyzw / p_xyzw.w();
	// Get 3 components
	p = vec3(p_xyzw.x(), p_xyzw.y(), p_xyzw.z());

	return true;
}

void Bezier::selection_button(int button, int action, const mat4 &view, const mat4 &projection) {
	static int x_pressed = 0, y_pressed = 0;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		int x = 0, y = 0;
		glfwGetMousePos(&x, &y);
		x_pressed = x; y_pressed = y;

		render_selection(view, projection);

		glFlush();
		glFinish();

		unsigned char res[4];
		glReadPixels(x, height - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);
		selected_point = res[0];

		if (selected_point >= 0 && selected_point < cam_pos_points.size())
			cam_pos_points[selected_point].selected() = true;

		if (selected_point >= cam_pos_points.size() && selected_point < cam_pos_points.size() + cam_look_points.size())
			cam_look_points[selected_point - cam_pos_points.size()].selected() = true;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		if (selected_point >= 0 && selected_point < cam_pos_points.size()) {
			cam_pos_points[selected_point].selected() = false;
		}

		if (selected_point >= cam_pos_points.size() && selected_point < cam_pos_points.size() + cam_look_points.size()) {
			cam_look_points[selected_point - cam_pos_points.size()].selected() = false;
		}

		int x = 0, y = 0;
		glfwGetMousePos(&x, &y);
		if (x == x_pressed && y == y_pressed) {
			return;
		}

		if (selected_point >= 0 && selected_point < cam_pos_points.size()) {
			unproject(x, y, cam_pos_points[selected_point].position(), view, projection);

			cam_pos_curve.set_points(cam_pos_points);
			cout << "cam_pos " << selected_point << " value : " << cam_pos_points[selected_point].position() << endl;
		}

		if (selected_point >= cam_pos_points.size() && selected_point < cam_pos_points.size() + cam_look_points.size()) {
			unproject(x, y, cam_look_points[selected_point - cam_pos_points.size()].position(), view, projection);

			cam_look_curve.set_points(cam_look_points);
			cout << "cam_look " << selected_point + cam_pos_points.size() << " value : " << cam_look_points[selected_point - cam_pos_points.size()].position() << endl;
		}
	}
}