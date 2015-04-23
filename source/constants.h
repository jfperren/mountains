#pragma once

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


typedef struct NoiseValues {
	NoiseType noise_type;
	Effect noise_effect;
	int width;
	int height;
	float amplitude;
	float offset;
	float seed;
};

typedef struct FractalValues {
	FractalType fractal_type;
	Effect fractal_effect;
	float H;
	int lacunarity;
	int octaves;
	float amplitude;
	float offset;
};