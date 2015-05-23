#version 330 core

in vec2 uv;

layout (location = 0) out vec4 height_out;
layout (location = 1) out vec4 water_out;
layout (location = 2) out vec4 sediment_out;
layout (location = 3) out vec4 flux_LR_out;
layout (location = 4) out vec4 flux_TB_out;
layout (location = 5) out vec4 velocity_out;

layout (location = 0) uniform sampler2D height_in;
layout (location = 1) uniform sampler2D water_in;
layout (location = 2) uniform sampler2D sediment_in;
layout (location = 3) uniform sampler2D flux_LR_in;
layout (location = 4) uniform sampler2D flux_TB_in;
layout (location = 5) uniform sampler2D velocity_in;

float erosion_deposition_rate;
float erosion_evaporation_rate;
float erosion_erosion_rate;
float erosion_rain_rate;

float erosion_sediment_capacity;
float erosion_direction_inertia;

uniform float DT;
uniform float DX;
uniform float DY;

const float g = 10;
const float A_l = 1;

const int RAINFALL = 0;
const int FLOW_FST = 1;
const int FLOW_SND = 2;
const int FLOW_THD = 3;
const int EROSION = 4;
const int TRANSPORT = 5;
const int EVAPORATION = 6;

void main() {
	
	float height = texture(height_in, uv)[0];
	float water = texture(water_in, uv)[0];
	float flux_L = texture(flux_LR_in, uv)[0];
	float flux_R = texture(flux_LR_in, uv)[1];
	float flux_T = texture(flux_TB_in, uv)[0];
	float flux_B = texture(flux_TB_in, uv)[1];
	float u = texture(velocity_in, uv)[0];
	float v = texture(velocity_in, uv)[1];

	vec2 uv_L = uv - vec2(DX, 0);
	vec2 uv_R = uv + vec2(DX, 0);
	vec2 uv_T = uv + vec2(0, DY);
	vec2 uv_B = uv - vec2(0, DY);

	// 1. Rainfall
	if (mode == RAINFALL) {
		float water = texture(water_in, uv)[0];
		water += erosion_rain_rate * DT;
	}

	// 2.1 Outflow flux update
	else if (mode == FLOW_FST) {
		float water_L = texture(water_in, uv_L)[0];
		float water_R = texture(water_in, uv_R)[0];
		float water_T = texture(water_in, uv_T)[0];
		float water_B = texture(water_in, uv_B)[0];

		float height_L = texture(height_in, uv_L)[0];
		float height_R = texture(height_in, uv_R)[0];
		float height_T = texture(height_in, uv_T)[0];
		float height_B = texture(height_in, uv_B)[0];

		float level_L = water_L + height_L;
		float level_R = water_R + height_R;
		float level_T = water_T + height_T;
		float level_B = water_B + height_B;

		float dl_L = level - level_L;
		float dl_R = level - level_R;
		float dl_T = level - level_T;
		float dl_B = level - level_B;

		flux_L = max(0, flux_L + DT * g * dl_L);
		flux_R = max(0, flux_R + DT * g * dl_R);
		flux_T = max(0, flux_T + DT * g * dl_T);
		flux_B = max(0, flux_B + DT * g * dl_B);

		float K = min(1, (water * DX * DY)/(DT * (flux_L + flux_R + flux_T + flux_B));

		flux_L = K * flux_L;
		flux_R = K * flux_R;
		flux_T = K * flux_T;
		flux_B = K * flux_B;
	}

	// 2.2 Water surface update
	else if (mode == FLOW_SCD) {

		float influx_L = texture(water_LR_in, uv_L)[1];
		float influx_R = texture(water_LR_in, uv_R)[0];
		float influx_B = texture(water_BT_in, uv_B)[1];
		float influx_T = texture(water_BT_in, uv_T)[0];

		float d = water;

		float water_amount = 0;
		water_amount += DT * (influx_L + influx_R + influx_B + influx_T);
		water_amount -= DT * (flux_L + flux_R + flux_B + flux_T);
		water_amount /= (DX * DY);

		water += water_amount;

		d += water;
		d /= 2;

		float flux_X = influx_L - flux_L + flux_R - influx_R;
		float flux_Y = influx_B - flux_B + flux_T - influx_T;

		u = flux_X / (d * DY);
		v = flux_Y / (d * DX);
	}

	// 3. Erosion and Deposition
	else if (mode == EROSION) {
		float alpha;
		float max_capacity = erosion_sediment_capacity * sin(alpha) * vec2(u, v).norm();

		if (sediment < max_capacity) {
			eroded_sediment = erosion_erosion_rate * (max_capacity - sediment);

			height -= eroded_sediment;
			sediment += eroded_sediment;
		} else {
			deposited_sediment = erosion_deposition_rate * (sediment - max_capacity);

			height += deposited_sediment;
			sediment -= deposited_sediment;
		}
	}

	// 4. Sediment Transportation
	else if (mode == TRANSPORT_AND_EVAPORATION) {

		vec2 uv_sed = uv - vec2(u * DT, v * DT);
		sediment_amount = texture(sediment_in, uv_sed);

	// 5. Evaporation
	
		water *= (1 - erosion_evaporation_rate * DT);
	}
}