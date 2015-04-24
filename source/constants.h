#pragma once

const string kHeaderString = "HEADER: terrain data";

typedef enum {
	COPY_TEXTURE,
	NO_NOISE,
	RANDOM_NOISE,
	PERLIN_NOISE,
	PERLIN_NOISE_ABS,
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

typedef struct GridParams {
	int grid_size;
};

typedef struct NoiseParams {
	NoiseType type;
	Effect effect;
	int width;
	int height;
	float amplitude;
	float offset;
	float seed;

	void setup(GLuint pid) {
		// Pass parameters to the shaders
		glUniform1f(glGetUniformLocation(pid, "noise_width"), width);
		glUniform1f(glGetUniformLocation(pid, "noise_height"), height);
		glUniform1f(glGetUniformLocation(pid, "noise_offset"), offset);
		glUniform1f(glGetUniformLocation(pid, "noise_amplitude"), amplitude);
		glUniform1i(glGetUniformLocation(pid, "noise_type"), type);
		glUniform1f(glGetUniformLocation(pid, "noise_seed"), seed);
		glUniform1i(glGetUniformLocation(pid, "noise_effect"), effect);
	}
};

typedef struct FractalParams {
	FractalType fractal_type;
	Effect fractal_effect;
	float H;
	int lacunarity;
	int octaves;
	float amplitude;
	float offset;
	bool enable;
};

typedef struct WaterParams {
	float height;
	float transparency;
	float depth_alpha_factor;
	float depth_color_factor;
	float reflection_factor;
	vec3 color;

	void setup(GLuint pid) {
		glUseProgram(pid);

		glUniform1f(glGetUniformLocation(pid, "water_height"), height);
		glUniform1f(glGetUniformLocation(pid, "water_transparency"), transparency);
		glUniform3f(glGetUniformLocation(pid, "water_color"), color[0], color[1], color[2]);
		glUniform1f(glGetUniformLocation(pid, "water_depth_alpha_factor"), depth_alpha_factor);
		glUniform1f(glGetUniformLocation(pid, "water_depth_color_factor"), depth_color_factor);
		glUniform1f(glGetUniformLocation(pid, "water_reflection_factor"), reflection_factor);
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
