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
	NoiseType noise_type;
	Effect noise_effect;
	int width;
	int height;
	float amplitude;
	float offset;
	float seed;
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
};