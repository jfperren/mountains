#pragma once
#include "icg_common.h"
#include <math.h>

class Trackball {
public:
    Trackball() : _radius(1.0f) {}

    // This function is called when the user presses the left mouse button down.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    void begin_drag(float x, float y) {
      _anchor_pos = vec3(x, y, 0.0f);
      project_onto_surface(_anchor_pos);
    }

    // This function is called while the user moves the curser around while the
    // left mouse button is still pressed.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    // Returns the rotation of the trackball in matrix form.
    mat4 drag(float x, float y) {
      	vec3 current_pos = vec3(x, y, 0.0f);
      	project_onto_surface(current_pos);

      	mat4 rotation = mat4::Identity();
      
      	// Rotation speed
      	float rotation_factor = 3;
      	// First we compute the rotation axis
      	vec3 axis = _anchor_pos.cross(current_pos);
	// The angle is found with arcsin
	float angle = rotation_factor * asin(axis.norm() / (_anchor_pos.norm() * current_pos.norm()));
	// Normalize the axis
	axis = axis.normalized();

	// Then we use the formulation of the rotation matrix around an axis :
	// http://fr.wikipedia.org/wiki/Matrice_de_rotation#Axe_de_rotation
	float u_x = axis[0];
	float u_y = axis[1];
	float u_z = axis[2];
	float c = cos(angle);
	float s = sin(angle);

	// First line
	rotation(0, 0) = pow(u_x, 2) + (1 - pow(u_x, 2)) * c;
	rotation(0, 1) = u_x * u_y * (1 - c) - u_z * s;
	rotation(0, 2) = u_x * u_z * (1 - c) + u_y * s;
	// second line
	rotation(1, 0) = u_x * u_y * (1 - c) + u_z * s;
	rotation(1, 1) = pow(u_y, 2) + (1 - pow(u_y, 2)) * c;
	rotation(1, 2) = u_y * u_z * (1 - c) - u_x * s;
	// Third line
	rotation(2, 0) = u_x * u_z * (1 - c) - u_y * s;
	rotation(2, 1) = u_y * u_z * (1 - c) + u_x * s;
	rotation(2, 2) = pow(u_z, 2) + (1 - pow(u_z, 2)) * c;
	
      	return rotation;
    }

private:
    // Projects the point p (whose z coordiante is still empty/zero) onto the
    // trackball surface. If the position at the mouse cursor is outside the
    // trackball, use a hyberbolic sheet as explained in:
    // https://www.opengl.org/wiki/Object_Mouse_Trackball.
    // The trackball radius is given by '_radius'.
    void project_onto_surface(vec3& p) const {
		if (pow(p[0], 2) + pow(p[1], 2) <= pow(_radius, 2) / 2){
			// On sphere surface
			p[2] = sqrt(pow(_radius, 2) - pow(p[0], 2) - pow(p[1], 2));
		}
		else {
			// On hyperbolic surface
			p[2] = pow(_radius, 2) / (2 * sqrt(pow(p[0], 2) + pow(p[1], 2)));
		}
    }

    float _radius;
    vec3 _anchor_pos;
    mat4 _rotation;
};
