#version 330 core

uniform mat4 mvp;

uniform float time;

in vec2 position;
out vec2 uv;

float PI = 3.14;

// Constants for the circular wave
float kMaxAmplitudeCircular = 0.12;
float kLambdaCircular = 0.5;
float kOmegaCircular = 2 * PI * 0.6;
vec2 center = vec2(1.25, 1.25);

// Constants for straight wave
float kAmplitudeStraight = 0.03;
float kOmegaStraight = PI * 0.6;
float kLambdaStraight = 0.5;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

	// Compute the distance from uv to center
	float radius = sqrt((uv[0] - center[0])*(uv[0] - center[0]) + (uv[1] - center[1])*(uv[1] - center[1]));

	// Then sets height as a circular sine wave function centered at 'center' and decreasing as 1/r^2
	float circular_wave = (kMaxAmplitudeCircular / (pow(radius, 2) + 1)) * cos(kOmegaCircular * time - 2 * PI / kLambdaCircular * radius);
    
	// Adds a longitudinal wave for more realistic effect
	float straight_wave = kAmplitudeStraight * cos(kOmegaStraight * time - 2 * PI / kLambdaStraight * (uv[0] - uv[1]));
	
	float height = circular_wave + straight_wave;

    vec3 pos_3d = vec3(position.x, height, -position.y);

    gl_Position = mvp * vec4(pos_3d, 1.0);
}
