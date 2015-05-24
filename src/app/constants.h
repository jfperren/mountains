#pragma once

const int WIDTH = 1600;
const int HEIGHT = 1000;

const string IO_HEADER_STRING = "HEADER: terrain data";

const int DRAW_NOISE = 0;
const int COPY_NOISE = 1;

const vec3 START_CAM_UP = vec3(0.0f, 1.0f, 0.0f);
const vec3 START_CAM_DIR = vec3(0.0f, -1.0f, -2.0f).normalized();
const vec3 START_CAM_POS = vec3(0.0f, 2.0f, 4.0f);

const float MAX_Y_CAM_DIR = 0.9;

const float DX = 0.01f;
const float DPHI = 1.0f;

const float NEAR = 0.1f;
const float FAR = 40.0f;

typedef enum {
	FREE,
	FPS,
	BEZIER,
	CAMERA_PATH
} NAVIGATION_MODE;

typedef enum {
	NO_NOISE,
	RANDOM_NOISE,
	PERLIN_NOISE,
	WORLEY_NOISE
} NoiseType;

typedef enum {
	FBM,
	MULTIFRACTAL,
	HYBRID // Need to add parameters
} FractalType;

typedef enum {
	NO_EFFECT,
	ABSOLUTE_VALUE,
	CLAMP_EXTREMAS, // Max/Min = 0/1
	DISCRETIZE // Step = 0.2
} Effect;

const static int PIXELS_PER_UNIT = 2048;

typedef struct WindowParams {
	int width;
	int height;
};

typedef struct GridParams {
	int length;
	int width;
	int resolution;

	void setup(GLuint pid){
		glUniform1i(glGetUniformLocation(pid, "grid_length"), length);
		glUniform1i(glGetUniformLocation(pid, "grid_width"), width);
		glUniform1i(glGetUniformLocation(pid, "grid_resolution"), resolution);
	}
};

typedef struct NoiseParams {
	NoiseType noise_type;
	FractalType fractal_type;

	Effect noise_effect;
	Effect fractal_effect;

	int width;
	int height;
	float amplitude;
	float offset;

	float H;
	int lacunarity;
	int octaves;

	float seed;

	NoiseParams copy() {
		return {
			noise_type,
			fractal_type,
			noise_effect,
			fractal_effect,
			width,
			height,
			amplitude,
			offset,
			H,
			lacunarity,
			octaves,
			seed
		};
	}

	void setup_copy(GLuint pid) {
		glUniform1f(glGetUniformLocation(pid, "fractal_offset"), offset);
		glUniform1f(glGetUniformLocation(pid, "fractal_amplitude"), amplitude);
		glUniform1i(glGetUniformLocation(pid, "fractal_type"), fractal_type);
		glUniform1i(glGetUniformLocation(pid, "fractal_effect"), fractal_effect);
	}

	void setup_draw(GLuint pid) {
		glUniform1i(glGetUniformLocation(pid, "noise_type"), noise_type);
		glUniform1i(glGetUniformLocation(pid, "noise_effect"), noise_effect);

		glUniform1f(glGetUniformLocation(pid, "noise_width"), width);
		glUniform1f(glGetUniformLocation(pid, "noise_height"), height);

		glUniform1f(glGetUniformLocation(pid, "noise_seed"), seed);

		glUniform1i(glGetUniformLocation(pid, "fractal_type"), fractal_type);
	}
};

static NoiseParams FLAT_NOISE = {
	NO_NOISE,
	FBM,
	NO_EFFECT,
	NO_EFFECT,
	1,
	1,
	0,
	1,
	0,
	0,
	0,
	0
};

typedef struct WaterParams {
	/* Water*/
	float height;
	float transparency;
	float depth_alpha_factor;
	float depth_color_factor;
	float reflection_factor;
	vec3 color;

	/* Waves */
	float waves_speed;
	float waves_tile_factor;
	float waves_amplitude;

	void setup(GLuint pid) {
		glUseProgram(pid);

		/* Water */
		glUniform1f(glGetUniformLocation(pid, "water_height"), height);
		glUniform1f(glGetUniformLocation(pid, "water_transparency"), transparency);
		glUniform3f(glGetUniformLocation(pid, "water_color"), color[0], color[1], color[2]);
		glUniform1f(glGetUniformLocation(pid, "water_depth_alpha_factor"), depth_alpha_factor);
		glUniform1f(glGetUniformLocation(pid, "water_depth_color_factor"), depth_color_factor);
		glUniform1f(glGetUniformLocation(pid, "water_reflection_factor"), reflection_factor);

		/* Waves */
		glUniform1f(glGetUniformLocation(pid, "waves_speed"), waves_speed);
		glUniform1f(glGetUniformLocation(pid, "waves_tile_factor"), waves_tile_factor);
		glUniform1f(glGetUniformLocation(pid, "waves_amplitude"), waves_amplitude);
	}
};

typedef struct LightParams {
	vec3 Ia;
	vec3 Id;
	vec3 position;

	void setup(GLuint pid){
		glUseProgram(pid);

		glUniform3fv(glGetUniformLocation(pid, "light_pos"), ONE, position.data());
		glUniform3fv(glGetUniformLocation(pid, "Ia"), ONE, Ia.data());
		glUniform3fv(glGetUniformLocation(pid, "Id"), ONE, Id.data());
	}
};

typedef struct TextureParams {
	float sand_min_height;
	float sand_max_height;
	float grass_max_height;
	
	float sand_max_slope;
	float grass_max_slope;

	int grass_h_transition;
	int grass_s_transition;
	int sand_h_transition;
	int sand_s_transition;

	void setup(GLuint pid) {
		glUniform1f(glGetUniformLocation(pid, "sand_min_height"), sand_min_height);
		glUniform1f(glGetUniformLocation(pid, "sand_max_height"), sand_max_height);
		glUniform1f(glGetUniformLocation(pid, "grass_max_height"), grass_max_height);

		glUniform1f(glGetUniformLocation(pid, "sand_max_slope"), sand_max_slope);
		glUniform1f(glGetUniformLocation(pid, "grass_max_slope"), grass_max_slope);

		glUniform1i(glGetUniformLocation(pid, "grass_h_transition"), grass_h_transition);
		glUniform1i(glGetUniformLocation(pid, "grass_s_transition"), grass_s_transition);
		glUniform1i(glGetUniformLocation(pid, "sand_h_transition"), sand_h_transition);
		glUniform1i(glGetUniformLocation(pid, "sand_s_transition"), sand_s_transition);
	}
};
